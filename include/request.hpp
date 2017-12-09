//
// Created by jestjest on 12/8/17.
//

#ifndef SIMPLE_KADEMLIA_REQUEST_HPP
#define SIMPLE_KADEMLIA_REQUEST_HPP

#include "node/nodeinfo.hpp"
#include "messages/message.hpp"

namespace kdml {
    class Request {

        NodeInfo dest;
        uint32_t tid;
        std::string msg;
        std::function<void(std::shared_ptr<kdml::net::Message> req)> onDone;
    };
}

#endif //SIMPLE_KADEMLIA_REQUEST_HPP
