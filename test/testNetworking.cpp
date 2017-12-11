//
// Created by jestjest on 12/2/17.
//

#include <cereal/archives/binary.hpp>
#include <messages/message.hpp>
#include <messages/pingQuery.hpp>
#include <node/kademlianode.hpp>
#include <messages/pingResponse.hpp>
#include <messages/findQuery.hpp>
#include <messages/findNodeResponse.hpp>
#include <messages/findValueResponse.hpp>
#include <messages/storeQuery.hpp>
#include <messages/storeResponse.hpp>

using namespace kdml::net;
using boost::asio::ip::udp;

void sendMessage(std::shared_ptr<Message> msg);

int main() {

    kdml::KademliaNode server("127.0.0.1", 5001);

    kdml::NodeInfo src = {"192.168.0.1", 8000};
    kdml::NodeInfo dest = {"192.168.0.2", 8000};
    std::shared_ptr<Message> ping = std::make_shared<PingQuery>(src.id, 1);
    sendMessage(ping);

    std::shared_ptr<Message> pong = std::make_shared<PingResponse>(dest.id, 1);
    sendMessage(pong);

    std::shared_ptr<Message> findNode = std::make_shared<FindQuery>(src.id, 2, src.id, QueryType::FIND_NODE);
    sendMessage(findNode);

    kdml::Nodes nodes = {{"192.168.0.3", 8000}, {"192.168.0.4", 8000}};
    std::shared_ptr<Message> findNodeRes = std::make_shared<FindNodeResponse>(dest.id, 2, nodes);
    sendMessage(findNodeRes);

    kdml::Nodes values = {{"192.168.0.3", 8000}, {"192.168.0.4", 8000}};
    std::shared_ptr<Message> findValueRes = std::make_shared<FindValueResponse>(dest.id, 2, true, nodes);
    sendMessage(findValueRes);

    mp::uint256_t someSongHash("98374983274");
    std::shared_ptr<Message> storeNode = std::make_shared<StoreQuery>(src.id, 3, someSongHash, src);
    sendMessage(storeNode);

    std::shared_ptr<Message> storeRes = std::make_shared<StoreResponse>(src.id, 3, true);
    sendMessage(storeRes);

    std::chrono::seconds duration(10);       // Let server receive the message.
    std::this_thread::sleep_for(duration);

    server.join();
}

void sendMessage(std::shared_ptr<Message> msg) {
    boost::asio::io_service ioService;
    udp::resolver resolver(ioService);
    udp::resolver::query query(udp::v4(), "127.0.0.1", "5001");
    udp::endpoint receiverEndpoint = *resolver.resolve(query);
    udp::socket socket(ioService);
    socket.open(udp::v4());

    boost::asio::streambuf buf;
    {
        std::ostream os(&buf);
        cereal::BinaryOutputArchive oarchive(os);
        oarchive(msg);
    }

    socket.send_to(buf.data(), receiverEndpoint);
}
