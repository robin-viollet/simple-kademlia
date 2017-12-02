//
// Created by jestjest on 11/11/17.
//

#ifndef SIMPLE_KADEMLIA_KADEMLIANODE_HPP
#define SIMPLE_KADEMLIA_KADEMLIANODE_HPP

#include "callbacks.hpp"
#include "kademliaprotocol.hpp"
#include <boost/asio/io_service.hpp>
#include <boost/thread.hpp>
#include <functional>
#include <vector>
#include <memory>
#include <thread>

namespace kdml {
    class KademliaNode {
        KademliaProtocol protocol;

        // TODO SHA 256 of more than just a string would be better.
        std::hash<std::string> hashCode;

    public:

        KademliaNode(std::string ipAddr, unsigned short port);

        // TODO Implement actual bootstrapping of all previous seen vs. assuming success.
        void bootstrap(KademliaNodeInfo peer);

        // TODO Wrap key in class
        void get(std::string key, GetCallback callback);

        void join();
    };
}

#endif //SIMPLE_KADEMLIA_KADEMLIANODE_HPP
