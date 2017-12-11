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

        uint32_t tid;
        boost::multiprecision::uint256_t key;
        bool findValue{false};
        std::string msg;
        std::function<void(std::shared_ptr<kdml::net::ResponseMessage> req)> onDone;

        Request(uint32_t tid,
                std::function<void(std::shared_ptr<kdml::net::ResponseMessage> req)> onDone) {
            this->tid = tid;
            this->onDone = onDone;
        }
    };
}

#endif //SIMPLE_KADEMLIA_REQUEST_HPP
