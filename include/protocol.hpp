#ifndef SIMPLE_KADEMLIA_PROTOCOL_HPP
#define SIMPLE_KADEMLIA_PROTOCOL_HPP

#include "node/nodeinfo.hpp"
#include "routing tree/routingtree.hpp"
#include "callbacks.hpp"
#include "network.hpp"
#include <boost/asio.hpp>
#include <thread>
#include <messages/responseMessage.hpp>
#include <messages/queryMessage.hpp>


namespace kdml {

    class Protocol {

        NodeInfo owner;
        Network network;
        RoutingTree routingTable;
        std::map<boost::multiprecision::uint256_t, Nodes> storage;

        boost::asio::io_service ioService;
        std::unique_ptr<boost::asio::io_service::work> ioLock;
        boost::asio::ip::udp::socket socket;
        boost::asio::ip::udp::endpoint remote;

        std::thread ioThread;

        void startReceive();
        void handleReceive(const boost::system::error_code& error,
                           std::size_t /*bytes_transferred*/);
        void handleMessage(std::shared_ptr<net::Message> msg);
        void handleQueryMessage(std::shared_ptr<net::QueryMessage> msg);
        void handleResponseMessage(std::shared_ptr<net::Message> msg);

        void probePeers(Nodes& endpoints);
        boost::system::error_code populateBuf(boost::asio::streambuf& sb);
        Nodes resolveEndpoint(const NodeInfo& ep);

        void refreshBuckets(RoutingTree::iterator startBucket);

    public:
        explicit Protocol(const NodeInfo& owner);

        void async_get(boost::multiprecision::uint256_t key, kdml::GetCallback callback);

        void bootstrap(const NodeInfo& peer);

        void join();
    };
}

#endif SIMPLE_KADEMLIA_PROTOCOL_HPP
