//
// Created by jestjest on 11/18/17.
//

#include <node/nodeinfo.hpp>
#include <messages/message.hpp>
#include <messages/pingQuery.hpp>
#include <messages/pingResponse.hpp>
#include <messages/findQuery.hpp>
#include <messages/findNodeResponse.hpp>
#include <messages/storeQuery.hpp>
#include <messages/findValueResponse.hpp>
#include <messages/storeResponse.hpp>

using namespace kdml::net;

void checkMessage(std::shared_ptr<Message> msg);

int main() {
    kdml::NodeInfo src = {"192.168.0.1", 8000};
    kdml::NodeInfo dest = {"192.168.0.2", 8000};
    std::shared_ptr<Message> ping = std::make_shared<PingQuery>(src.id, 1);
    checkMessage(ping);

    std::shared_ptr<Message> pong = std::make_shared<PingResponse>(dest.id, 1);
    checkMessage(pong);

    std::shared_ptr<Message> findNode = std::make_shared<FindQuery>(src.id, 2, src.id, QueryType::FIND_NODE);
    checkMessage(findNode);

    kdml::Nodes nodes = {{"192.168.0.3", 8000}, {"192.168.0.4", 8000}};
    std::shared_ptr<Message> findNodeRes = std::make_shared<FindNodeResponse>(dest.id, 2, nodes);
    checkMessage(findNodeRes);

    kdml::Nodes values = {{"192.168.0.3", 8000}, {"192.168.0.4", 8000}};
    std::shared_ptr<Message> findValueRes = std::make_shared<FindValueResponse>(dest.id, 2, true, nodes);
    checkMessage(findValueRes);

    mp::uint256_t someSongHash("98374983274");
    std::shared_ptr<Message> storeNode = std::make_shared<StoreQuery>(src.id, 3, someSongHash, src);
    checkMessage(storeNode);

    std::shared_ptr<Message> storeRes = std::make_shared<StoreResponse>(src.id, 3, true);
    checkMessage(storeRes);
}

void checkMessage(std::shared_ptr<Message> msg) {

    std::stringstream ss;       // any stream can be used
    std::shared_ptr<Message> after;
    {
        cereal::BinaryOutputArchive oarchive(ss);     // create output archive
        oarchive(msg);     // write data to archive
    }   // archive going out of scope flushes content

    {
        cereal::BinaryInputArchive iarchive(ss);
        // De-serialize the data as base class pointers, and watch as they are
        // re-instantiated as derived classes
        iarchive(after);
    }

    std::cout << "Before: " << *msg << std::endl;
    std::cout << "After: " << *after << std::endl;
    std::cout << std::endl;
}


