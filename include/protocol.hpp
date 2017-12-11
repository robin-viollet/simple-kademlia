#ifndef SIMPLE_KADEMLIA_PROTOCOL_HPP
#define SIMPLE_KADEMLIA_PROTOCOL_HPP

#include "node/nodeinfo.hpp"
#include "routing tree/routingtree.hpp"
#include "callbacks.hpp"
#include <boost/asio.hpp>
#include <thread>
#include <messages/responseMessage.hpp>
#include <messages/queryMessage.hpp>

namespace kdml {

    namespace net {
        class Network;
    }
    // number of concurrent lookups in node lookup
    const int a = 1;

    class Protocol {

        NodeInfo owner;
        net::Network* network;
        RoutingTree routingTable;

        std::map<boost::multiprecision::uint256_t, Nodes> storage;

        boost::asio::io_service ioService;
        std::unique_ptr<boost::asio::io_service::work> ioLock;

        std::thread ioThread;

        void handleMessage(std::shared_ptr<net::Message> msg, NodeInfo* sender);
        void handleQuery(std::shared_ptr<net::QueryMessage> msg, NodeInfo* sender);
        void handleResponse(std::shared_ptr<net::ResponseMessage> msg);

        //map of get/put request ids -> callback for get, put requests
        //copy node lookup procedure passing pointer to heap allocated queue, request id

        void probePeers(Nodes endpoints);
        Nodes resolveEndpoint(const NodeInfo& ep);

//        void refreshBuckets(RoutingTree::iterator startBucket);

    public:
        explicit Protocol(const NodeInfo& owner);

        void async_get(boost::multiprecision::uint256_t key, kdml::GetCallback callback);
<<<<<<< HEAD

=======
//        void lookup_node(boost::multiprecision::uint256_t key);
>>>>>>> 2cffb1903fe9732a68e1ea7e9eba531c3c3c390d
        void bootstrap(const NodeInfo& peer);
        void join();

<<<<<<< HEAD
        void lookup_node(boost::multiprecision::uint256_t key);
=======
        void handleReceive(const boost::system::error_code& error,
                           std::size_t /*bytes_transferred*/);
>>>>>>> 2cffb1903fe9732a68e1ea7e9eba531c3c3c390d
    };
}

#endif //SIMPLE_KADEMLIA_PROTOCOL_HPP
