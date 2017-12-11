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
//                    doLookUp(owner.id, [bucket](Nodes& found) {
//                        refreshBuckets(bucket);
//                    });
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

    bool node_closer(boost::multiprecision::uint256_t key, NodeInfo a, NodeInfo b) {
        return (key ^ a.id) < (key ^ b.id);
    }

    void find_value_callback(Nodes nodes, boost::multiprecision::uint256_t key, bool found, GetCallback callback) {
        if(found) {
            callback(nodes);
        } else {
            callback(std::vector<NodeInfo>());
        }
    }

    void Protocol::async_get(mp::uint256_t key, kdml::GetCallback callback) {
        Nodes a_closest_nodes = routingTable.getAClosestNodes(a, key);
        for(NodeInfo node : a_closest_nodes) {
            network->send_find_value(key, node, find_value_callback, callback);
        }
    }

    void Protocol::async_store(boost::multiprecision::uint256_t key, NodeInfo value) {
        Nodes a_closest_nodes = routingTable.getAClosestNodes(a, key);
        for(NodeInfo node : a_closest_nodes) {
            network->send_store(key, node);
        }
    }

    /*
     * Pick a closest nodes to key from bucket, send asynchronous FIND_NODE rpcs to each.
     * Pass heap allocated queue to each
     */
    void Protocol::node_lookup(boost::multiprecision::uint256_t key, kdml::GetCallback callback) {
//        this->callback = callback;
//        std::vector<NodeInfo> a_closest_nodes = routingTable.getAClosestNodes(a, key);
//        auto key_comp = [key](NodeInfo a, NodeInfo b) {
//            // Distance of node from key defined as XOR value.
//            return node_closer(key, a, b);
//        };
//        k_nodes_queue = new std::priority_queue<NodeInfo, std::vector<NodeInfo>, decltype(key_comp)>(key_comp);
//        std::priority_queue<NodeInfo> *queue = (std::priority_queue<NodeInfo> *)k_nodes_queue;
//        for(NodeInfo node : a_closest_nodes) {
//            queue->push(node);
//            responses_waiting++;
//            network->send_find_node(key, node, find_node_callback);
//        }
//        if (responses_waiting == 0) {
//            callback(a_closest_nodes);
//        }
    }

    void find_node_callback(std::vector<NodeInfo> k_closest_nodes, boost::multiprecision::uint256_t key,
                                      kdml::GetCallback callback, void *k_nodes_queue) {
//        std::priority_queue<NodeInfo> *queue = (std::priority_queue<NodeInfo> *)k_nodes_queue;
//        responses_waiting--;
//        NodeInfo closest = queue->top();
//        // todo: This could send rpcs to all k nodes, not just a nodes. Need to set nodeinfo's as queried.
//        // also need to check which nodes have responded
//        for(NodeInfo node : k_closest_nodes) {
//            if (node_closer(key, node, closest)) {
//                queue->push(node);
//                responses_waiting++;
//                network->send_find_node(key, node, find_node_callback);
//            }
//        }
//
//        // If all nodes queried have responded, return k_closest_nodes.
//        // todo: stop waiting after amount of time?
//        if (responses_waiting == 0) {
//            std::vector<NodeInfo> k_nodes;
//            while(!queue->empty() && k_nodes.size() < k) {
//                k_nodes.push_back(queue->top());
//                queue->pop();
//            }
//            callback(k_nodes);
//        }

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
            outstanding.onDone(msg);
        }
    }
}
