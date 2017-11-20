//
// Created by jestjest on 11/19/17.
//

#include "kademliaprotocol.hpp"

namespace kdml {
    KademliaProtocol::KademliaProtocol(KademliaNodeInfo owner) : owner(std::move(owner)) {}

    void KademliaProtocol::sendMessage(net::Message& msg, KademliaNodeInfo& dest) {

    }
}


