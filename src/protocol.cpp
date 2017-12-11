//
// Created by jestjest on 11/19/17.
//

#include <boost/bind.hpp>
#include <cereal/archives/binary.hpp>
#include <messages/queryMessage.hpp>
#include <messages/responseMessage.hpp>
#include <messages/findQuery.hpp>
#include <messages/storeQuery.hpp>
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

    void Protocol::async_get(mp::uint256_t key, kdml::GetCallback callback) {
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

//    void lookup_node(boost::multiprecision::uint256_t key) {
//        std::vector<NodeInfo*> a_closest_nodes = routingTable.getAClosestNodes(a, key);
//        std::priority_queue
//        uint256_t distance = key ^ node_id;
//
//    }

    void Protocol::handleReceive(const boost::system::error_code& error,
                                         std::size_t /*bytes_transferred*/) {

        if (error == asio::error::operation_aborted) {
            return;
        }
        std::cout << "Received something" << std::endl;

        asio::streambuf sb;
        boost::system::error_code recvError = network->populateBuf(sb);

        if (!recvError) {
            NodeInfo* sender = new NodeInfo(network->getRemotePeer());
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
                    auto* endpoint = new NodeInfo(ep);
                    auto bucket = routingTable.insertNode(endpoint);
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

//    void Protocol::refreshBuckets(RoutingTree::iterator start) {
//        for (; start != routingTable.end(); start++) {
//            mp::uint256_t randomId = routingTable.randomId(start);
//            doLookUp(randomId, [](...){});
//        }
//    }

    void Protocol::handleMessage(std::shared_ptr<net::Message> msg,
                                 NodeInfo* sender) {

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

    void Protocol::lookup_node(boost::multiprecision::uint256_t key) {
        std::vector<NodeInfo*> a_closest_nodes = routingTable.getAClosestNodes(a, key);
//        std::priority_queue
//        mp::uint256_t distance = key ^ node_id;

    }


    void Protocol::handleQuery(std::shared_ptr<net::QueryMessage> msg,
                               NodeInfo* sender) {

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
                               [](NodeInfo* node) { return *node; }
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
                                   [](NodeInfo* node) {
                                       return *node;
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
