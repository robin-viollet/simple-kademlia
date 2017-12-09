//
// Created by jestjest on 12/8/17.
//

#ifndef SIMPLE_KADEMLIA_REQUEST_HPP
#define SIMPLE_KADEMLIA_REQUEST_HPP

#include "node/nodeinfo.hpp"
#include "messages/message.hpp"

namespace kdml {
    struct Request {

        //todo: don't we only need callback?

        NodeInfo dest;
        uint32_t tid;
        std::string msg;
        std::function<void(std::shared_ptr<kdml::net::Message> req)> onDone;

        Request(NodeInfo dest, uint32_t tid, std::function<void(std::shared_ptr<kdml::net::Message> req)> onDone) {
            this.dest = dest;
            this.tid = tid;
            this.onDone = onDone;
        }
    };
}

#endif //SIMPLE_KADEMLIA_REQUEST_HPP
