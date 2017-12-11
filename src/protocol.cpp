//
// Created by jestjest on 11/19/17.
//

#include <boost/bind.hpp>
#include <cereal/archives/binary.hpp>
#include <messages/queryMessage.hpp>
#include <messages/responseMessage.hpp>
#include <messages/findQuery.hpp>
#include <messages/storeQuery.hpp>
#include <queue>
#include <messages/findValueResponse.hpp>
#include <messages/findNodeResponse.hpp>
#include "protocol.hpp"
#include "network.hpp"


namespace kdml {

    using boost::asio::ip::udp;
    namespace asio = boost::asio;
    namespace mp = boost::multiprecision;

    Protocol::Protocol(const NodeInfo& node)
            : owner(node),
              routingTable(node.id),
              ioService{},
              ioLock(new asio::io_service::work(ioService)) {

        network = new net::Network(node, ioService, this);
        network->startReceive();
        ioThread = std::thread([this]() { ioService.run(); });
    }

    void Protocol::join() {
        ioService.stop();
        if (ioThread.joinable()) {
            ioThread.join();
        }
    }

    // TODO Asynchronous bootstrap (queue messages while bootstrapping).
    void Protocol::bootstrap(const NodeInfo& peer) {
        Nodes endpoints = resolveEndpoint(peer);
        probePeers(std::move(endpoints));
    }

    void Protocol::handleReceive(const boost::system::error_code& error,
                                 std::size_t /*bytes_transferred*/) {

        if (error == asio::error::operation_aborted) {
            return;
        }
        std::cout << "Received something" << std::endl;

        asio::streambuf sb;
        boost::system::error_code recvError = network->populateBuf(sb);

        if (!recvError) {
            NodeInfo sender(network->getRemotePeer());
            std::shared_ptr<kdml::net::Message> message;
            {
                std::istream istream(&sb);
                cereal::BinaryInputArchive iarchive(istream);
                iarchive(message);
            }

            std::cout << "Parsed message: " << *message << std::endl;
            handleMessage(message, sender);
        }
        network->startReceive();
    }

    void Protocol::probePeers(Nodes endpoints) {
        if (endpoints.empty()) {
            throw "Failed to find peers";
        } else {
            NodeInfo ep = endpoints.back();
            endpoints.pop_back();

            SimpleCallback onPong = [=](bool failure) {
                if (failure) {
                    probePeers(endpoints);
                } else {
                    auto bucket = routingTable.insertNode(ep);
                    node_lookup(owner.id, [bucket](Nodes found) {
                    }, false);
                }
            };

            network->send_ping(ep, onPong);
        }
    }

    Nodes Protocol::resolveEndpoint(const NodeInfo& endpoint) {
        udp::resolver resolver(ioService);
        udp::resolver::query query(udp::v4(), endpoint.getIpAddr(),
                                   std::to_string(endpoint.port));
        Nodes eps;
        auto resolvedEndpoints = resolver.resolve(query);
        decltype(resolvedEndpoints) end;

        while (resolvedEndpoints != end) {
            auto ep = (*resolvedEndpoints).endpoint();
            eps.emplace_back(NodeInfo{ep.address().to_string(), ep.port()});
            resolvedEndpoints++;
        }
        return eps;
    }

    void Protocol::handleMessage(std::shared_ptr<net::Message> msg,
                                 NodeInfo sender) {

        switch (msg->getMessageType()) {
            case net::MessageType::ERROR: {
                std::cerr << "Received error message: " << *msg << std::endl;
                break;
            }
            case net::MessageType::QUERY: {
                routingTable.insertNode(sender);
                handleQuery(std::dynamic_pointer_cast<net::QueryMessage>(msg), sender);
                break;
            }
            case net::MessageType::RESPONSE: {
                routingTable.insertNode(sender);
                handleResponse(std::dynamic_pointer_cast<net::ResponseMessage>(msg));
                break;
            }
        }
    }

    void find_value_callback(Nodes nodes, boost::multiprecision::uint256_t key, bool found, GetCallback callback) {
        if(found) {
            callback(nodes);
        } else {
            callback(std::vector<NodeInfo>());
        }
    }

    void Protocol::async_get(mp::uint256_t key, kdml::GetCallback callback) {
//        Nodes a_closest_nodes = routingTable.getAClosestNodes(a, key);
//        for(NodeInfo node : a_closest_nodes) {
//            network->send_find_value(key, node);
//        }
        node_lookup(key, callback, true);
    }

    void Protocol::async_store(boost::multiprecision::uint256_t key, NodeInfo value) {
//        Nodes a_closest_nodes = routingTable.getAClosestNodes(a, key);
//        for(NodeInfo node : a_closest_nodes) {
//            network->send_store(key, node);
//        }
        node_lookup(key, NULL, false);
    }

    void Protocol::store_callback(boost::multiprecision::uint256_t key, Nodes nodes) {

        for(NodeInfo node : nodes) {
            std::cout << "Storing value in node with ip addr " << node.ipAddr << " and id " << node.id
                      << " for key " << key << std::endl;
            network->send_store(key, node);
        }
    }


    /*
     * Pick a closest nodes to key from bucket, send asynchronous FIND_NODE rpcs to each.
     */
    void Protocol::node_lookup(boost::multiprecision::uint256_t key, kdml::GetCallback callback, bool findValue) {

        std::vector<NodeInfo> a_closest_nodes = routingTable.getAClosestNodes(a, key);
        RequestState request_state;
        request_state.key = key;
        request_state.findValue = findValue;
        request_state.callback = callback;
        for(NodeInfo node : a_closest_nodes) {
            std::cout << "Looked up node with ip addr " << node.ipAddr << " and id " << node.id
                      << " for key " << key << std::endl;
            NodeInfoWrapper node_wrapper(key, node);
            request_state.k_closest_nodes.push(node_wrapper);
            request_state.responses_waiting++;
            if (request_state.findValue) {
                network->send_find_value(key, node);
            } else {
                network->send_find_node(key, node);
            }

        }
        lookups[key] = request_state;

    }

    void Protocol::node_lookup_callback(std::vector<NodeInfo> k_closest_nodes,
                                        boost::multiprecision::uint256_t key) {

        auto it = lookups.find(key);
        if (it != lookups.end()) {
            RequestState request_state = it->second;
            request_state.responses_waiting--;
            NodeInfoWrapper closest = request_state.k_closest_nodes.top();
            for(NodeInfo node : k_closest_nodes) {
                NodeInfoWrapper node_wrapper(key, node);
                //todo: fix
//                request_state.unqueried_nodes.push(node);
                request_state.k_closest_nodes.push(node_wrapper);
                if (request_state.node_comp(node_wrapper, closest)) {
                    std::cout << "Node is closer" << std::endl;
                    request_state.responses_waiting++;
                    if (request_state.findValue) {
                        network->send_find_value(key, node);
                    } else {
                        network->send_find_node(key, node);
                    }
                }
            }
            std::cout << "responses waiting " << request_state.responses_waiting << std::endl;
            if (request_state.responses_waiting == 0) {
                std::vector<NodeInfo> k_nodes;
                while(!request_state.k_closest_nodes.empty() && k_nodes.size() < k) {
                    k_nodes.push_back(request_state.k_closest_nodes.top().node);
                    request_state.k_closest_nodes.pop();
                }
                if (request_state.findValue) {
                    request_state.callback(k_nodes);
                } else {
                    //store query
                    store_callback(key, k_nodes);
                }

                lookups.erase ( it, lookups.end() );
            }

        } else {
            //todo:
        }

    }


    void Protocol::handleQuery(std::shared_ptr<net::QueryMessage> msg,
                               NodeInfo sender) {

        switch (msg->getQueryType()) {
            case net::QueryType::PING: {
                network->send_ping_response(sender, msg->getTid());
                break;
            }
            case net::QueryType::FIND_NODE: {
                auto query = std::dynamic_pointer_cast<net::FindQuery>(msg);
                auto nodes = routingTable.getKClosestNodes(query->getTarget());
                Nodes result;
                result.resize(nodes.size());

                std::transform(nodes.begin(), nodes.end(), result.begin(),
                               [](NodeInfo node) { return node; }
                );

                network->send_find_node_response(sender, result, msg->getTid());
                break;
            }
            case net::QueryType::FIND_VALUE: {
                auto query = std::dynamic_pointer_cast<net::FindQuery>(msg);
                auto value = storage.find(query->getTarget());
                bool found = (value != storage.end());
                Nodes result;
                if (found) {
                    result = value->second;
                } else {
                    auto nodes = routingTable.getKClosestNodes(query->getTarget());
                    std::transform(nodes.begin(), nodes.end(), result.begin(),
                                   [](NodeInfo node) {
                                       return node;
                                   });
                }
                network->send_find_value_response(sender, found, result,
                                                  msg->getTid());
                break;
            }
            case net::QueryType::STORE: {
                auto query = std::dynamic_pointer_cast<net::StoreQuery>(msg);
                bool success = false;

                try {
                    storage[query->getKey()].emplace_back(query->getVal());
                    success = true;
                } catch (...) {}

                network->send_store_response(sender, success, msg->getTid());
                break;
            }
        }
    }

    void Protocol::handleResponse(std::shared_ptr<net::ResponseMessage> msg) {
        if (!network->containsReq(msg->getTid())) {
            std::cerr << "Received alien message" << *msg << std::endl;
        } else {
            Request outstanding = network->getRequest(msg->getTid());
//            outstanding.onDone(msg);
            auto it = lookups.find(outstanding.key);
            if (it != lookups.end()) {
                RequestState request_state = it->second;
                if (outstanding.findValue) {
                    net::FindValueResponse &value_res = dynamic_cast<net::FindValueResponse&>(*msg);
                    node_lookup_callback(value_res.data, outstanding.key);
                } else {
                    net::FindNodeResponse &find_res = dynamic_cast<net::FindNodeResponse&>(*msg);
                    node_lookup_callback(find_res.nodes, outstanding.key);
                }
            }
        }
    }
}
