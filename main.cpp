//
// Created by jestjest on 11/18/17.
//

#include <iostream>
#include <fstream>
#include "messages.hpp"
#include <cereal/archives/binary.hpp>

int main() {
    std::stringstream ss;   // any stream can be used
    {
        cereal::BinaryOutputArchive oarchive(ss);     // create output archive

        KademliaNodeInfo src("127.0.0.1", 8000, "1");
        KademliaNodeInfo dest("127.0.0.1", 8001, "2");
        // Create instances of the derived classes, but only keep base class pointers
        std::shared_ptr<kdml::net::Message> ptr1 = std::make_shared<kdml::net::PingMessage>(src, dest);
        oarchive(ptr1);     // write data to archive
    }   // archive going out of scope flushes content

    {
        cereal::BinaryInputArchive iarchive(ss);

        // De-serialize the data as base class pointers, and watch as they are
        // re-instantiated as derived classes
        std::shared_ptr<kdml::net::Message> ptr1;
        iarchive(ptr1);

        // Ta-da! This should output:
        std::cout << "Type: " << static_cast<int>(ptr1->getType()) << std::endl;
        ptr1->printContents();
    }
    return 0;
}
