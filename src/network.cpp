//
// Created by jestjest on 12/8/17.
//

#include "network.hpp"

/* TODO: user passes local IP or we decide?
 * boost::asio::io_service io_service;
 * boost::asio::ip::tcp::resolver resolver(io_service);
 * boost::asio::ip::tcp::resolver::query query(boost::asio::ip::host_name(), "");
 * boost::asio::ip::tcp::resolver::iterator it = resolver.resolve(query);
 * boost::asio::ip::tcp::endpoint endpoint = *it;
 */


void send_ping(NodeInfo dest, SimpleCallback onPong) {
    long tid = next_tid++;
    Request request = new Request(tid, [onPong] (std::shared_ptr<kdml::net::Message> req) {
        //todo: parse message and call onPong
    });
    std::shared_ptr<Message> ping_message = std::make_shared<PingMessage>(owner.id, tid);
    send_RPC(dest, ping_message);
    requests.insert(make_pair(tid, request));
}

void send_find_node(uint256_t key, NodeInfo dest, GetCallback onComplete) {
    long tid = next_tid++;
    Request request = new Request(tid, [onComplete] (std::shared_ptr<kdml::net::Message> req) {
        //todo: parse message and call onComplete
    });
    std::shared_ptr<Message> find_node_message = std::make_shared<FindNodeMessage>(owner.id, tid, key);
    send_RPC(dest, find_node_message);
    requests.insert(make_pair(tid, request));
}

void send_find_value(uint256_t key, NodeInfo dest, GetCallback onComplete) {
    long tid = next_tid++;
    Request request = new Request(tid, [onComplete] (std::shared_ptr<kdml::net::Message> req) {
        //todo: parse message and call onComplete
    });
    std::shared_ptr<Message> find_value_message = std::make_shared<FindValueMessage>(owner.id, tid, key);
    send_RPC(dest, find_value_message);
    requests.insert(make_pair(tid, request));
}

void send_store(uint256_t key, std::string value, NodeInfo dest) {
    long tid = next_tid++;
    Request request = new Request(tid, [onComplete] (std::shared_ptr<kdml::net::Message> req) {
        //empty callback
    });
    std::shared_ptr<Message> store_message = std::make_shared<StoreMessage>(owner.id, tid, key, value);
    send_RPC(dest, store_message);
    requests.insert(make_pair(tid, request));
}

void send_RPC(NodeInfo dest, std::shared_ptr <Message> message) {
    unsigned long ip_addr = dest.ipAddr;
    unsigned short port = dest.port;

    boost::asio::io_service ioService;
    udp::resolver resolver(ioService);
    udp::resolver::query query(udp::v4(), ip_addr, port);
    udp::endpoint receiverEndpoint = *resolver.resolve(query);
    udp::socket socket(ioService);
    socket.open(udp::v4());

    boost::asio::streambuf buf;
    {
        std::ostream os(&buf);
        cereal::BinaryOutputArchive oarchive(os);
        oarchive(message);
    }

    socket.send_to(buf.data(), receiverEndpoint);
}