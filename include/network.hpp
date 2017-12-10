//
// Created by jestjest on 12/8/17.
//

#ifndef SIMPLE_KADEMLIA_NETWORK_HPP
#define SIMPLE_KADEMLIA_NETWORK_HPP

#include <map>
#include <utility>
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

        void send_RPC(NodeInfo dest, std::shared_ptr<net::Message> message);

    public:

        explicit Network(NodeInfo node) {
            this->owner = std::move(node);
            this->next_tid = 0;
        }

        bool containsReq(uint32_t tid);
        Request getReq(uint32_t tid);

        void send_ping(NodeInfo dest, SimpleCallback onPong);
        void send_find_node(mp::uint256_t key, NodeInfo dest, GetCallback onComplete);
        void send_find_value(mp::uint256_t key, NodeInfo dest, GetCallback onComplete);
        void send_store(mp::uint256_t key, NodeInfo dest);

        void send_ping_response(NodeInfo dest, uint32_t tid);
        void send_find_node_response(NodeInfo dest, Nodes nodes, uint32_t tid);
        void send_find_value_response(NodeInfo dest, bool success, Nodes result, uint32_t tid);
        void send_store_response(NodeInfo dest, bool success, uint32_t tid);
    };
}

#endif //SIMPLE_KADEMLIA_NETWORK_HPP