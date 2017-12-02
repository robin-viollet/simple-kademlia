//
// Created by jestjest on 11/19/17.
//

#include <boost/bind.hpp>
#include <cereal/archives/binary.hpp>
#include "kademliaprotocol.hpp"


namespace kdml {

    KademliaProtocol::KademliaProtocol(const KademliaNodeInfo& node)
            : owner(std::move(node)), ioService(),
              socket(ioService, udp::endpoint(udp::v4(), node.port)) {

        startReceive();

        ioLock = boost::asio::io_service::work(ioService);
        ioThread = std::thread([this]() { ioService.run(); });
    }

    void KademliaProtocol::async_get(boost::multiprecision::uint256_t key, kdml::GetCallback callback) {

    }

    void KademliaProtocol::join() {
        ioLock.reset();
        if (ioThread.joinable()) {
            ioThread.join();
        }
    }

    void KademliaProtocol::startReceive() {
        socket.async_receive(
                boost::asio::null_buffers(),
                boost::bind(&handleReceive, this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
    }

    void KademliaProtocol::handleReceive(const boost::system::error_code& error,
                                         std::size_t /*bytes_transferred*/) {

        if (error == boost::asio::error::operation_aborted) {
            return;
        }

        size_t available = socket.available();
        boost::asio::streambuf sb;
        auto buffer = sb.prepare(available);
        boost::system::error_code recvError;
        size_t bytesRead = socket.receive_from(buffer, remoteEndpoint, 0, recvError);
        sb.commit(bytesRead);

        if (recvError == boost::asio::error::message_size) {
            std::cout << "Message was too large for some reason." << std::endl;
        } else if (!recvError) {

            std::shared_ptr<kdml::net::Message> message;
            std::istream istream(&sb);
            cereal::BinaryInputArchive iarchive(istream);
            iarchive(message);
            std::cout << "Received: " << message << std::endl;

            // Now send a message back.

        }
        startReceive();
    }

}


