//
// Created by jestjest on 11/18/17.
//

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <node/nodeinfo.hpp>
#include <messages/message.hpp>
#include <messages/pingMessage.hpp>
#include <node/kademlianode.hpp>


TEST_CASE("cereal-serialize-deserialize", "[cereal]") {

    using namespace kdml::net;

    std::stringstream ss;       // any stream can be used
    std::shared_ptr<Message> before;
    std::shared_ptr<Message> after;

    {
        cereal::BinaryOutputArchive oarchive(ss);     // create output archive

        kdml::NodeInfo src = {"192.168.0.1", 8000};
        // Create instances of the derived classes, but only keep base class pointers
        before = std::make_shared<PingMessage>(src.id, 1);
        oarchive(before);     // write data to archive
    }   // archive going out of scope flushes content

    {
        cereal::BinaryInputArchive iarchive(ss);

        // De-serialize the data as base class pointers, and watch as they are
        // re-instantiated as derived classes
        iarchive(after);
    }

    std::shared_ptr<PingMessage> beforePing = std::dynamic_pointer_cast<PingMessage>(before);
    std::shared_ptr<PingMessage> afterPing = std::dynamic_pointer_cast<PingMessage>(before);

    ss.str("");
    ss << *before;
    std::string beforeStr = ss.str();

    ss.str("");
    ss << *after;
    std::string afterStr = ss.str();

    REQUIRE(static_cast<int>(beforePing->getMessageType()) == static_cast<int>(afterPing->getMessageType()));
    REQUIRE(static_cast<int>(beforePing->getQueryType()) == static_cast<int>(afterPing->getQueryType()));
    REQUIRE(beforeStr == afterStr);

    std::cout << afterStr << std::endl;
}


