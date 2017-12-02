//
// Created by jestjest on 11/19/17.
//

#include <boost/bind.hpp>
#include <cereal/archives/binary.hpp>
#include "kademliaprotocol.hpp"
#include "messages/message.hpp"


namespace kdml {

    using udp = boost::asio::ip::udp;

    KademliaProtocol::KademliaProtocol(const KademliaNodeInfo& node)
            : owner(node), ioService{},
              ioLock(new boost::asio::io_service::work(ioService)),
              socket(ioService, udp::endpoint(udp::v4(), node.port)) {

        startReceive();
        ioThread = std::thread([this]() { ioService.run(); });
    }

    void KademliaProtocol::async_get(boost::multiprecision::uint256_t key, kdml::GetCallback callback) {

    }

    void KademliaProtocol::join() {
        ioService.stop();
        if (ioThread.joinable()) {
            ioThread.join();
        }
    }

    void KademliaProtocol::startReceive() {
        socket.async_receive(
                boost::asio::null_buffers(),
                boost::bind(&kdml::KademliaProtocol::handleReceive, this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
    }

    void KademliaProtocol::handleReceive(const boost::system::error_code& error,
                                         std::size_t /*bytes_transferred*/) {

        if (error == boost::asio::error::operation_aborted) {
            return;
        }

        std::cout << "Received something" << std::endl;

        size_t available = socket.available();
        boost::asio::streambuf sb;
        auto buffer = sb.prepare(available);
        boost::system::error_code recvError;
        size_t bytesRead = socket.receive_from(buffer, remoteEndpoint, 0, recvError);
        sb.commit(bytesRead);

        std::cout << "Read datagram of size(bytes) " << bytesRead << std::endl;

        if (!recvError) {

            std::shared_ptr<kdml::net::Message> message;
            std::istream istream(&sb);
            cereal::BinaryInputArchive iarchive(istream);
            iarchive(message);
            std::cout << "Parsed message: " << *message.get() << std::endl;

            // Now send a message back.

        }
        startReceive();
    }

}
