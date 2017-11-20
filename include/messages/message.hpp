//
// Created by jestjest on 11/19/17.
//

#ifndef INC_244B_FINAL_MESSAGE_HPP
#define INC_244B_FINAL_MESSAGE_HPP

#include "kademlianodeinfo.hpp"
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
        };
    }
}

#endif //INC_244B_FINAL_MESSAGE_HPP
