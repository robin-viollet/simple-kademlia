//
// Created by jestjest on 12/8/17.
//

#ifndef SIMPLE_KADEMLIA_NETWORK_HPP
#define SIMPLE_KADEMLIA_NETWORK_HPP

#include <map>
#include "node/nodeinfo.hpp"
#include "request.hpp"
#include "callbacks.hpp"
#include "routing tree/routingtree.hpp"

namespace kdml {
    class Network {

        NodeInfo owner;
        uint32_t next_tid;
        //map tids to requests to lookup callbacks later
        std::map<uint32_t, Request> requests;

        void send_RPC();

    public:
        Network(NodeInfo node) {
            this->owner = node;
            this->next_tid = 0;
        }

        void send_ping(NodeInfo dest, SimpleCallback onPong);
        void send_find_node(uint256_t key, NodeInfo dest, GetCallback onComplete);
        void send_find_value(uint256_t key, NodeInfo dest, GetCallback onComplete);
        void send_store(uint256_t key,  NodeInfo dest);
    };
}

#endif //SIMPLE_KADEMLIA_NETWORK_HPP