/*
 *
 * This defines a library using boost::asio and cereal that sends messages
 * as UDP packets. Each Kademlia node uses this library to communicate
 * with each other.
 *
 * Example usage:
 * ---------------------
 * protocol.sendMessage(KademliaNodeInfo dest, kdml::net::Message message);
 * protocol.sendMessage(KademliaNodeInfo dest, kdml::net::Message message);
 */

#include "node/kademlianodeinfo.hpp"
#include "routingtree.hpp"
#include "callbacks.hpp"
#include <boost/asio.hpp>
#include <messages/message.hpp>
#include <thread>



namespace kdml {

    class KademliaProtocol {

        using namespace boost::asio::ip;

        KademliaNodeInfo owner;
        RoutingTree routingTree;

        boost::asio::io_service ioService;
        boost::optional<boost::asio::io_service::work> ioLock;
        udp::socket socket;
        udp::endpoint remoteEndpoint;


        // TODO Multiple threads, but maybe not needed.
        std::thread ioThread;
        void startReceive();

    public:
        KademliaProtocol(const KademliaNodeInfo& owner);

        void async_get(boost::multiprecision::uint256_t key, kdml::GetCallback callback);

        void join();

        void handleReceive(const boost::system::error_code& error,
                           std::size_t /*bytes_transferred*/);
    };
}

//
//// RPCs
//// returns k KademliaNodeInfo structs closest to key/ID, NOT RECURSIVE
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
//// returns k KademliaNodeInfo structs closest to key
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
