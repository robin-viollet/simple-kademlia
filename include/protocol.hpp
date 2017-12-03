#ifndef SIMPLE_KADEMLIA_PROTOCOL_HPP
#define SIMPLE_KADEMLIA_PROTOCOL_HPP

#include "node/nodeinfo.hpp"
#include "routingtree.hpp"
#include "callbacks.hpp"
#include <boost/asio.hpp>
#include <thread>


namespace kdml {

    class Protocol {

        NodeInfo owner;
        RoutingTree routingTree;

        boost::asio::io_service ioService;
        std::unique_ptr<boost::asio::io_service::work> ioLock;
        boost::asio::ip::udp::socket socket;
        boost::asio::ip::udp::endpoint remote;

        std::thread ioThread;
        void startReceive();

        void probePeers(std::vector<NodeInfo> endpoints, SimpleCallback onComplete);
        boost::system::error_code populateBuf(boost::asio::streambuf& sb);
        std::vector<NodeInfo> resolveEndpoint(const NodeInfo& ep);

    public:
        explicit Protocol(const NodeInfo& owner);

        void async_get(boost::multiprecision::uint256_t key, kdml::GetCallback callback);

        void bootstrap(const NodeInfo& peer);

        void join();

        void handleReceive(const boost::system::error_code& error,
                           std::size_t /*bytes_transferred*/);
    };
}

#endif SIMPLE_KADEMLIA_PROTOCOL_HPP

//
//// RPCs
//// returns k NodeInfo structs closest to key/ID, NOT RECURSIVE
//// callback: contains (value, list of k nodes)
//findNode(key);
//findValue(key);  // If receiver has id in hashtable, return value.
//
//// empty callback
//ping();
//
//// stores key value at given node
//store(key, value);
//
//// returns k NodeInfo structs closest to key
//// A = 1
//findKClosestNodes(id) {
//        // keep track of closest node
//        // keep PriorityQueue of k closest nodes not queried
//
//        // pick A nodes from closest non-empty bucket
//            // pick from next closest buckets until A nodes
//        // send findNode RPC to A nodes
//
//        // on response from each node, pick A nodes from k received not yet queried
//            // send findNode RPC again to A nodes
//            // if these don't return closer nodes, query first k in PQueue
//        // finish when have received responses from k closest nodes
//}
