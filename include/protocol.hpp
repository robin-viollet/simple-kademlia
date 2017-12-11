#ifndef SIMPLE_KADEMLIA_PROTOCOL_HPP
#define SIMPLE_KADEMLIA_PROTOCOL_HPP

#include "node/nodeinfo.hpp"
#include "routing tree/routingtree.hpp"
#include "callbacks.hpp"
#include <boost/asio.hpp>
#include <thread>
#include <messages/responseMessage.hpp>
#include <messages/queryMessage.hpp>
#include <queue>

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

        void handleMessage(std::shared_ptr<net::Message> msg, NodeInfo sender);
        void handleQuery(std::shared_ptr<net::QueryMessage> msg, NodeInfo sender);
        void handleResponse(std::shared_ptr<net::ResponseMessage> msg);

        //map of get/put request ids -> callback for get, put requests
        //copy node lookup procedure passing pointer to heap allocated queue, request id

        void probePeers(Nodes endpoints);
        Nodes resolveEndpoint(const NodeInfo& ep);

        //todo: map of key -> dynamic priority queue for node lookup, callback
        std::priority_queue<NodeInfo, std::vector<NodeInfo>, bool(*)(NodeInfo, NodeInfo)> *k_nodes_queue;
        GetCallback callback;
        int responses_waiting = 0;
        void find_node_callback(std::vector<NodeInfo > k_closest_nodes, boost::multiprecision::uint256_t key);

    public:
        explicit Protocol(const NodeInfo& owner);

        void async_get(boost::multiprecision::uint256_t key, kdml::GetCallback callback);

        void bootstrap(const NodeInfo& peer);
        void join();

        void node_lookup(boost::multiprecision::uint256_t key, GetCallback callback);
        void handleReceive(const boost::system::error_code& error,
                           std::size_t /*bytes_transferred*/);



    };
}

#endif //SIMPLE_KADEMLIA_PROTOCOL_HPP
