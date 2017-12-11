//
// Created by jestjest on 12/8/17.
//

#include <messages/findQuery.hpp>
#include <messages/pingQuery.hpp>
#include <messages/storeQuery.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/bind.hpp>
#include <messages/pingResponse.hpp>
#include <messages/findNodeResponse.hpp>
#include <messages/storeResponse.hpp>
#include <messages/findValueResponse.hpp>
#include "network.hpp"
#include "protocol.hpp"

/* TODO: user passes local IP or we decide?
 * boost::asio::io_service io_service;
 * boost::asio::ip::tcp::resolver resolver(io_service);
 * boost::asio::ip::tcp::resolver::query query(boost::asio::ip::host_name(), "");
 * boost::asio::ip::tcp::resolver::iterator it = resolver.resolve(query);
 * boost::asio::ip::tcp::endpoint endpoint = *it;
 */

namespace kdml {
    namespace net {

        using boost::asio::ip::udp;
        namespace asio = boost::asio;

        Network::Network(NodeInfo node, asio::io_service &ioService, Protocol *protocol)
                : owner(std::move(node)), next_tid(0), requests{}, protocol(protocol),
                  socket(ioService, udp::endpoint(udp::v4(), node.port)) {}

        void Network::startReceive() {
            socket.async_receive(
                    asio::null_buffers(),
                    boost::bind(&kdml::Protocol::handleReceive, protocol,
                                asio::placeholders::error,
                                asio::placeholders::bytes_transferred));
        }

        boost::system::error_code Network::populateBuf(asio::streambuf &sb) {
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

        NodeInfo Network::getRemotePeer() {
            return {remote.address().to_string(), remote.port()};
        }

        bool Network::containsReq(uint32_t tid) {
            return requests.find(tid) != requests.end();
        }

        Request Network::getRequest(uint32_t tid) {
            return requests.at(tid);
        }

        void Network::send_ping_response(NodeInfo dest, uint32_t tid) {
            std::shared_ptr<Message> pong = std::make_shared<PingResponse>(owner.id, tid);
            send_RPC(dest, pong);
        }

        void Network::send_find_node_response(NodeInfo dest, Nodes nodes, uint32_t tid) {
            std::shared_ptr<Message> findNodeRes = std::make_shared<FindNodeResponse>(owner.id, tid, nodes);
            send_RPC(dest, findNodeRes);
        }

        void Network::send_store_response(NodeInfo dest, bool success, uint32_t tid) {
            std::shared_ptr<Message> storeRes = std::make_shared<StoreResponse>(owner.id, tid, success);
            send_RPC(dest, storeRes);
        }

        void Network::send_find_value_response(NodeInfo dest, bool success, Nodes result, uint32_t tid) {
            std::shared_ptr<Message> valueRes = std::make_shared<FindValueResponse>(owner.id, tid, success, result);
            send_RPC(dest, valueRes);
        }


        void Network::send_ping(NodeInfo dest, SimpleCallback onPong) {
            long tid = next_tid++;
            std::shared_ptr<net::Message> ping_message = std::make_shared<net::PingQuery>(owner.id, tid);
            send_RPC(dest, ping_message);
            Request request(tid, [onPong](std::shared_ptr<net::ResponseMessage> res) {
                PingResponse &node_res = dynamic_cast<net::PingResponse&>(*res);
                onPong(true);
            });
            requests.insert(std::make_pair(tid, request));
        }

        void Network::send_find_node(mp::uint256_t key, NodeInfo dest, FindNodeCallback onComplete) {
            long tid = next_tid++;
            std::shared_ptr<net::Message> find_node_message = std::make_shared<net::FindQuery>(owner.id, tid, key, QueryType::FIND_NODE);
            send_RPC(dest, find_node_message);
            Request request(tid, [onComplete](std::shared_ptr<net::ResponseMessage> res) {
                FindNodeResponse &node_res = dynamic_cast<net::FindNodeResponse&>(*res);
                onComplete(node_res.nodes, node_res.id);
            });
            requests.insert(std::make_pair(tid, request));
        }

        void Network::send_find_value(mp::uint256_t key, NodeInfo dest, FindValueCallback onComplete, GetCallback callback) {
            long tid = next_tid++;
            std::shared_ptr<net::Message> find_value_message = std::make_shared<net::FindQuery>(owner.id, tid, key, QueryType::FIND_VALUE);
            send_RPC(dest, find_value_message);
            Request request(tid, [onComplete, callback](std::shared_ptr<net::ResponseMessage> res) {
                FindValueResponse &value_res = dynamic_cast<net::FindValueResponse&>(*res);
                onComplete(value_res.data, value_res.id, value_res.found, callback);
            });
            requests.insert(std::make_pair(tid, request));
        }

        void Network::send_store(mp::uint256_t key, NodeInfo dest) {
            long tid = next_tid++;
            std::shared_ptr<net::Message> store_message = std::make_shared<net::StoreQuery>(owner.id, tid, key, owner);
            send_RPC(dest, store_message);
            Request request(tid, [](std::shared_ptr<net::Message> res) {
                //empty callback
            });
            requests.insert(std::make_pair(tid, request));
        }

        void Network::send_RPC(NodeInfo dest, std::shared_ptr<net::Message> message) {
            boost::asio::ip::udp::endpoint destination(
                    boost::asio::ip::address::from_string(dest.getIpAddr()), dest.port);

            boost::asio::streambuf sb;
            {
                std::ostream os(&sb);
                cereal::BinaryOutputArchive oarchive(os);
                oarchive(message);
            }

            socket.async_send_to(sb.data(), destination,
                                 boost::bind(&Network::handleSend, this, message,
                                             boost::asio::placeholders::error,
                                             boost::asio::placeholders::bytes_transferred));
        }

        void Network::handleSend(std::shared_ptr<Message> msg, const boost::system::error_code& error, std::size_t) {
            if (error) {
                std::cerr << "Error sending message: " << *msg << std::endl;
            } else if (msg->getMessageType() == MessageType::RESPONSE) {
                std::cout << "Sent response: " << *msg << std::endl;
                // do nothing
            } else if (msg->getMessageType() == MessageType::QUERY){
                std::cout << "Sent query: " << *msg << std::endl;
                // TODO
                // construct requests prior to send_RPC and pass them to send_RPC and handleSend
                // insert request into map
            }
        }
    }
}