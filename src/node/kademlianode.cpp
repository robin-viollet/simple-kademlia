//
// Created by jestjest on 11/27/17.
//

#include "node/kademlianode.hpp"

kdml::KademliaNode::KademliaNode(std::string ipAddr, unsigned short port)
        : protocol({ipAddr, port}), hashCode() {
}

void kdml::KademliaNode::bootstrap(kdml::KademliaNodeInfo peer) {

}

void kdml::KademliaNode::join() {
    protocol.join();
}

void kdml::KademliaNode::get(std::string key, kdml::GetCallback callback) {
    boost::multiprecision::uint256_t keyHash = hashCode(key);
    protocol.async_get(keyHash, callback);
}
