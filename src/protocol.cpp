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


namespace kdml {

    using boost::asio::ip::udp;
    namespace asio = boost::asio;
    namespace mp = boost::multiprecision;

    Protocol::Protocol(const NodeInfo& node)
            : owner(node),
              routingTable(node.id),
              network(node),
              ioService{},
              ioLock(new asio::io_service::work(ioService)),
              socket(ioService, udp::endpoint(udp::v4(), node.port)) {

        startReceive();
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
        probePeers(endpoints);
    }

    void Protocol::startReceive() {
        socket.async_receive(
                asio::null_buffers(),
                boost::bind(&kdml::Protocol::handleReceive, this,
                            asio::placeholders::error,
                            asio::placeholders::bytes_transferred));
    }

    void Protocol::handleReceive(const boost::system::error_code& error,
                                         std::size_t /*bytes_transferred*/) {

        if (error == asio::error::operation_aborted) {
            return;
        }
        std::cout << "Received something" << std::endl;

        asio::streambuf sb;
        boost::system::error_code recvError = populateBuf(sb);

        if (!recvError) {
            std::shared_ptr<kdml::net::Message> message;
            {
                std::istream istream(&sb);
                cereal::BinaryInputArchive iarchive(istream);
                iarchive(message);
            }

            std::cout << "Parsed message: " << *message << std::endl;
            handleMessage(message);
        }
        startReceive();
    }

    void Protocol::probePeers(Nodes& endpoints) {
        if (endpoints.empty()) {
            throw "Failed to find peers";       // TODO Custom error codes.
        } else {
            NodeInfo ep = endpoints.back();
            endpoints.pop_back();

            SimpleCallback onPong = [=](bool failure) {
                if (failure) {
                    probePeers(endpoints);
                } else {
                    NodeInfo* endpoint = new NodeInfo(ep);
                    auto bucket = routingTable.insertNode(endpoint);
                    doLookUp(owner.id, [bucket](Nodes& found) {
                        refreshBuckets(bucket);
                    });
                }
            };

            network.send_ping(ep, onPong);
        }
    }

    boost::system::error_code Protocol::populateBuf(asio::streambuf& sb) {
        size_t available = socket.available();
        auto buffer = sb.prepare(available);
        boost::system::error_code recvError;
        size_t bytesRead = socket.receive_from(buffer, remote, 0, recvError);
        sb.commit(bytesRead);

        if (!recvError) {
            std::cout << "Read " << bytesRead << " bytes" << std::endl;
        } else {
            std::cout << "Error: " << recvError.message() << std::endl;
        }

        return recvError;
    }

    Nodes Protocol::resolveEndpoint(const NodeInfo& ep) {
        udp::resolver resolver(ioService);
        udp::resolver::query query(udp::v4(),
                                   ep.getIpAddr(), std::to_string(ep.port));
        Nodes endpoints;
        auto resolvedEndpoints = resolver.resolve(query);
        decltype(resolvedEndpoints) end;

        while (resolvedEndpoints != end) {
            auto ep = (*resolvedEndpoints).endpoint();
            endpoints.emplace_back({ep.address().to_string(), ep.port()});
            resolvedEndpoints++;
        }
        return endpoints;
    }

    void Protocol::refreshBuckets(RoutingTree::iterator start) {
        for (; start != routingTable.end(); start++) {
            mp::uint256_t randomId = routingTable.randomId(start);
            doLookUp(randomId, [](...){});
        }
    }

    void Protocol::handleMessage(std::shared_ptr<net::Message> msg) {
        switch (msg->getMessageType()) {
            case net::MessageType::ERROR: {
                std::cerr << "Received error message: " << *msg << std::endl;
                break;
            }
            case net::MessageType::QUERY: {
                handleQueryMessage(std::dynamic_pointer_cast<net::QueryMessage>(msg));
                break;
            }
            case net::MessageType::RESPONSE: {
                handleResponseMessage(msg);
                break;
            }
        }
    }

    void Protocol::handleQueryMessage(std::shared_ptr<net::QueryMessage> msg) {
        NodeInfo* peer = new NodeInfo(remote.address().to_string(), remote.port());
        routingTable.insertNode(peer);

        switch (msg->getQueryType()) {
            case net::QueryType::PING: {
                network.send_ping_response(*peer, msg->getTid());
                break;
            }
            case net::QueryType::FIND_NODE: {
                auto query = std::dynamic_pointer_cast<net::FindQuery>(msg);
                auto nodes = routingTable.getKClosestNodes(query->getTarget());
                Nodes result;
                std::transform(nodes.begin(), nodes.end(), result.begin(),
                               [](NodeInfo* node) {
                                   return *node;
                               });

                network.send_find_node_response(*peer, result, msg->getTid());
                break;
            }
            case net::QueryType::FIND_VALUE: {
                auto query = std::dynamic_pointer_cast<net::FindQuery>(msg);
                auto found = storage.find(query->getTarget());
                Nodes result;
                if (found != storage.end()) {
                    result = found->second;
                } else {
                    auto nodes = routingTable.getKClosestNodes(query->getTarget());
                    std::transform(nodes.begin(), nodes.end(), result.begin(),
                                   [](NodeInfo* node) {
                                       return *node;
                                   });
                }
                network.send_find_value_response(*peer, found != storage.end(),
                                                 result, msg->getTid());
                break;
            }
            case net::QueryType::STORE: {
                auto query = std::dynamic_pointer_cast<net::StoreQuery>(msg);
                bool success = false;
                try {
                    storage[query->getKey()].emplace_back(query->getVal());
                    success = true;
                } catch (...) {}
                network.send_store_response(*peer, success, msg->getTid());
                break;
            }
        }
    }

    void Protocol::handleResponseMessage(std::shared_ptr<net::Message> msg) {
        if (!network.containsReq(msg->getTid())) {
            std::cerr << "Received alien message" << *msg << std::endl;
        } else {
            Request outstanding = network.getReq(msg->getTid());
            outstanding.onDone(msg);
        }
    }
}
