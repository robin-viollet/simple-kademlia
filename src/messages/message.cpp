//
// Created by jestjest on 11/19/17.
//

#include "messages/message.hpp"

namespace kdml {
    namespace net {

        kdml::net::Message::Message(kdml::net::MessageType type,
                                    kdml::KademliaNodeInfo src,
                                    kdml::KademliaNodeInfo dest) :
                type(type), src(std::move(src)), dest(std::move(dest)) {}

        kdml::net::MessageType kdml::net::Message::getType() const {
            return type;
        }

    }
}