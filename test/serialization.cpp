//
// Created by jestjest on 11/18/17.
//

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <routing tree/kademlianodeinfo.hpp>
#include <messages/message.hpp>
#include <messages/pingMessage.hpp>
#include "cereal/archives/binary.hpp"


TEST_CASE("Serialize-Deserialize") {

    std::stringstream ss;       // any stream can be used
    std::shared_ptr<kdml::net::Message> before;
    std::shared_ptr<kdml::net::Message> after;

    {
        cereal::BinaryOutputArchive oarchive(ss);     // create output archive

        kdml::KademliaNodeInfo src = {"127.0.0.1", 8000, "1"};
        kdml::KademliaNodeInfo dest = {"127.0.0.1", 8001, "2"};
        // Create instances of the derived classes, but only keep base class pointers
        before = std::make_shared<kdml::net::PingMessage>(src, dest);
        oarchive(before);     // write data to archive
    }   // archive going out of scope flushes content

    {
        cereal::BinaryInputArchive iarchive(ss);

        // De-serialize the data as base class pointers, and watch as they are
        // re-instantiated as derived classes
        iarchive(after);
    }

    ss.str("");
    ss << *before;
    std::string beforeStr = ss.str();

    ss.str("");
    ss << *after;
    std::string afterStr = ss.str();

    REQUIRE(static_cast<int>(before->getType()) == static_cast<int>(after->getType()));
    REQUIRE(beforeStr == afterStr);
}
