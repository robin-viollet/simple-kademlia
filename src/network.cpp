//
// Created by jestjest on 12/8/17.
//

#include <messages/findQuery.hpp>
#include <messages/pingQuery.hpp>
#include <messages/storeQuery.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/bind.hpp>
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

        void Network::send_ping_response(NodeInfo *dest, uint32_t tid) {
        }

        void Network::send_find_node_response(NodeInfo *dest, Nodes nodes, uint32_t tid) {

        }

        void Network::send_store_response(NodeInfo *dest, bool success, uint32_t tid) {

        }

        void Network::send_ping(NodeInfo dest, SimpleCallback onPong) {
            long tid = next_tid++;
            Request request(dest, tid, [onPong](std::shared_ptr<net::Message> res) {
    //            //todo: parse message and call onPong
            });
            std::shared_ptr<net::Message> ping_message = std::make_shared<net::PingQuery>(owner.id, tid);
            send_RPC(dest, ping_message);
            requests.insert(std::make_pair(tid, request));
        }

        void Network::send_find_node(mp::uint256_t key, NodeInfo dest, GetCallback onComplete) {
//        long tid = next_tid++;
//        Request request = new Request(tid, [onComplete](std::shared_ptr<net::Message> res) {
//            //todo: parse message and call onComplete
//        });
//        std::shared_ptr<net::Message> find_node_message = std::make_shared<net::FindQuery>(owner.id, tid, key);
//        send_RPC(dest, find_node_message);
//        requests.insert(std::make_pair(tid, request));
        }

        void Network::send_find_value(mp::uint256_t key, NodeInfo dest, GetCallback onComplete) {
//        long tid = next_tid++;
//        Request request = new Request(tid, [onComplete](std::shared_ptr<net::Message> res) {
//            //todo: parse message and call onComplete
//        });
//        std::shared_ptr<net::Message> find_value_message = std::make_shared<net::FindQuery>(owner.id, tid, key);
//        send_RPC(dest, find_value_message);
//        requests.insert(std::make_pair(tid, request));
        }

        void Network::send_store(mp::uint256_t key, NodeInfo dest) {
//        long tid = next_tid++;
//        Request request = new Request(tid, [](std::shared_ptr<net::Message> res) {
//            //empty callback
//        });
//        std::shared_ptr<net::Message> store_message = std::make_shared<net::StoreQuery>(owner.id, tid, key, value);
//        send_RPC(dest, store_message);
//        requests.insert(std::make_pair(tid, request));
        }

        void Network::send_RPC(NodeInfo dest, std::shared_ptr<net::Message> message) {
//        std::string ip_addr = dest.getIpAddr();
//        unsigned short port = dest.port;
//
//        boost::asio::io_service ioService;
//        udp::resolver resolver(ioService);
//        udp::resolver::query query(udp::v4(), ip_addr, std::to_string(port));
//        udp::endpoint receiverEndpoint = *resolver.resolve(query);
//        udp::socket socket(ioService);
//        socket.open(udp::v4());
//
//        boost::asio::streambuf buf;
//        {
//            std::ostream os(&buf);
//            cereal::BinaryOutputArchive oarchive(os);
//            oarchive(message);
//        }
//
//        socket.send_to(buf.data(), receiverEndpoint);
        }
    }
}