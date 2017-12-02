//
// Created by jestjest on 11/19/17.
//

#ifndef SIMPLE_KADEMLIA_MESSAGE_HPP
#define SIMPLE_KADEMLIA_MESSAGE_HPP

#include "node/kademlianodeinfo.hpp"
#include "messages/messageType.hpp"

namespace kdml {
    namespace net {

        class Message {
        protected:
            MessageType type;
            KademliaNodeInfo src;
            KademliaNodeInfo dest;
        public:
            Message(MessageType type,
                    KademliaNodeInfo src, KademliaNodeInfo dest);

            MessageType getType() const;
            virtual void print(std::ostream&) const = 0;

            friend std::ostream& operator<<(std::ostream& stream, Message& msg) {
                msg.print(stream);
                return stream;
            }

            template <class Archive>
            void serialize(Archive& ar) {
                ar(src, dest);
            }
        };
    }
}

#endif //SIMPLE_KADEMLIA_MESSAGE_HPP
