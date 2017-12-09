//
// Created by jestjest on 12/8/17.
//

#ifndef SIMPLE_KADEMLIA_NETWORK_HPP
#define SIMPLE_KADEMLIA_NETWORK_HPP

#include <map>
#include "node/nodeinfo.hpp"
#include "request.hpp"
#include "callbacks.hpp"
#include "routingtree.hpp"

namespace kdml {
    class Network {

        NodeInfo owner;
        long nextTid;
        std::map<uint32_t, Request> requests;

    public:
        Network(NodeInfo node, RoutingTree routingTable);
        void sendPing(NodeInfo dest, SimpleCallback onPong);
        void sendFindNode(Id id, NodeInfo dest, GetCallback onComplete);
    };
}

#endif //SIMPLE_KADEMLIA_NETWORK_HPP
