//
// Created by jestjest on 11/19/17.
//

#include <boost/bind.hpp>
#include <cereal/archives/binary.hpp>
#include "protocol.hpp"
#include "messages/message.hpp"


namespace kdml {

    using boost::asio::ip::udp;
    namespace asio = boost::asio;
    namespace mp = boost::multiprecision;

    Protocol::Protocol(const NodeInfo& node)
            : owner(node), ioService{},
              ioLock(new asio::io_service::work(ioService)),
              socket(ioService, udp::endpoint(udp::v4(), node.port)) {

        startReceive();
        ioThread = std::thread([this]() { ioService.run(); });
    }

    void Protocol::async_get(mp::uint256_t key, kdml::GetCallback callback) {
    }

    void Protocol::join() {
        ioService.stop();
        if (ioThread.joinable()) {
            ioThread.join();
        }
    }

    // TODO Asynchronous bootstrap (queue messages while bootstrapping).
    void Protocol::bootstrap(const NodeInfo& peer) {
        std::vector<NodeInfo> endpoints = resolveEndpoint(peer);

        SimpleCallback onComplete = [this](bool failure) {
            if (failure)
                throw "Failed to find peers";       // TODO Custom error codes.
//            refreshBuckets();
        };

        probePeers(endpoints, std::move(onComplete));

//        send FIND_NODE(local)  -> peer
//        populate table with found nodes
//        callback:
//        for each other kbucket {
//                look up random key in that bucket
//                send FIND_NODE(key) -> closest node to key
//                populate table with found nodes
//        }
    }

    void Protocol::startReceive() {
        socket.async_receive(
                asio::null_buffers(),
                boost::bind(&kdml::Protocol::handleReceive, this,
                            asio::placeholders::error,
                            asio::placeholders::bytes_transferred));
    }

    void Protocol::handleReceive(const boost::system::error_code& error,
                                         std::size_t /*bytes_transferred*/) {

        if (error == asio::error::operation_aborted) {
            return;
        }
        std::cout << "Received something" << std::endl;

        asio::streambuf sb;
        boost::system::error_code recvError = populateBuf(sb);

        if (!recvError) {
            std::shared_ptr<kdml::net::Message> message;
            {
                std::istream istream(&sb);
                cereal::BinaryInputArchive iarchive(istream);
                iarchive(message);
            }

            std::cout << "Parsed message: " << *message.get() << std::endl;
            // Now interpret the message.
        }
        startReceive();
    }

    void Protocol::probePeers(std::vector<NodeInfo> endpoints,
                                      SimpleCallback onComplete) {
        if (endpoints.empty()) {
            onComplete(true);
        } else {

        }
    }

    boost::system::error_code Protocol::populateBuf(asio::streambuf& sb) {
        size_t available = socket.available();
        auto buffer = sb.prepare(available);
        boost::system::error_code recvError;
        size_t bytesRead = socket.receive_from(buffer, remote, 0, recvError);
        sb.commit(bytesRead);

        if (!recvError) {
            std::cout << "Read " << bytesRead << " bytes" << std::endl;
        } else {
            std::cout << "Error: " << recvError.message() << std::endl;
        }

        return recvError;
    }

    std::vector<NodeInfo> Protocol::resolveEndpoint(const NodeInfo& ep) {
        udp::resolver resolver(ioService);
        udp::resolver::query query(udp::v4(), ep.getIpAddr(), std::to_string(ep.port));
        std::vector<NodeInfo> endpoints;
        auto resolvedEndpoints = resolver.resolve(query);
        decltype(resolvedEndpoints) end;

        while (resolvedEndpoints != end) {
            auto endpoint = (*resolvedEndpoints).endpoint();
            endpoints.emplace_back(endpoint.address().to_string(),
                                   endpoint.port());
            resolvedEndpoints++;
        }
        return endpoints;
    }

}
