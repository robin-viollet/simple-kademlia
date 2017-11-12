//
// Created by jestjest on 11/11/17.
//

#ifndef SIMPLE_KADEMLIA_KADEMLIANODEINFO_HPP
#define SIMPLE_KADEMLIA_KADEMLIANODEINFO_HPP

#include <boost/multiprecision/cpp_int.hpp>

struct KademliaNodeInfo {
    // ip address
    short port;
    boost::multiprecision::uint256_t nodeID;

    KademliaNodeInfo() {

    }
};

#endif //SIMPLE_KADEMLIA_KADEMLIANODEINFO_HPP
