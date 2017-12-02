//
// Created by jestjest on 11/19/17.
//

#ifndef SIMPLE_KADEMLIA_MESSAGETYPE_HPP
#define SIMPLE_KADEMLIA_MESSAGETYPE_HPP

namespace kdml {
    namespace net {
        enum class MessageType {
            PING,
            REPLY,
            FIND_NODE,
            FIND_VALUE,
            STORE,
        };
    }
}

#endif //SIMPLE_KADEMLIA_MESSAGETYPE_HPP
