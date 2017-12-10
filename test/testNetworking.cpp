//
// Created by jestjest on 12/2/17.
//

#include "catch.hpp"

#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <messages/message.hpp>
#include <messages/pingQuery.hpp>
#include <node/kademlianode.hpp>

using namespace kdml::net;
using boost::asio::ip::udp;

std::shared_ptr<Message> makePing(const std::string& fromAddr,
                                  unsigned short fromPort) {
    kdml::NodeInfo src = {fromAddr, fromPort};
    return std::make_shared<PingQuery>(src.id, 1337);
}

TEST_CASE("messaging-send ping", "[messaging]") {
    kdml::KademliaNode server("127.0.0.1", 5001);

    boost::asio::io_service ioService;
    udp::resolver resolver(ioService);
    udp::resolver::query query(udp::v4(), "127.0.0.1", "5001");
    udp::endpoint receiverEndpoint = *resolver.resolve(query);
    udp::socket socket(ioService);
    socket.open(udp::v4());

    std::shared_ptr<Message> ping = makePing("192.168.0.1", 5000);
    boost::asio::streambuf buf;
    {
        std::ostream os(&buf);
        cereal::BinaryOutputArchive oarchive(os);
        oarchive(ping);
    }

    socket.send_to(buf.data(), receiverEndpoint);

    std::chrono::seconds duration(1);       // Let server receive the message.
    std::this_thread::sleep_for(duration);

    server.join();
}

