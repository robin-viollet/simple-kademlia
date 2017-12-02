//
// Created by jestjest on 12/2/17.
//

#include "catch.hpp"

#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <messages/message.hpp>
#include <messages/pingMessage.hpp>
#include <node/kademlianode.hpp>

using namespace kdml::net;
using boost::asio::ip::udp;

std::shared_ptr<Message> makePing(const std::string& fromAddr,
                                  unsigned short fromPort,
                                  const std::string& toAddr,
                                  unsigned short toPort) {
    kdml::KademliaNodeInfo src = {fromAddr, fromPort};
    kdml::KademliaNodeInfo dest = {toAddr, toPort};
    return std::make_shared<PingMessage>(src, dest);
}

TEST_CASE("Send ping message", "[messaging]") {
    kdml::KademliaNode server("127.0.0.1", 5001);

    boost::asio::io_service io_service;
    udp::resolver resolver(io_service);
    udp::resolver::query query(udp::v4(), "127.0.0.1", "5001");
    udp::endpoint receiver_endpoint = *resolver.resolve(query);
    udp::socket socket(io_service);
    socket.open(udp::v4());

    std::shared_ptr<Message> ping = makePing("127.0.0.1", 5000, "127.0.0.1", 5001);
    boost::asio::streambuf buf;
    {
        std::ostream os(&buf);
        cereal::BinaryOutputArchive oarchive(os);
        oarchive(ping);
    }

    socket.send_to(buf.data(), receiver_endpoint);

    std::chrono::seconds duration(1);
    std::this_thread::sleep_for(duration);

    server.join();
}

