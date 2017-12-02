//
// Created by jestjest on 11/19/17.
//

#include "messages/pingMessage.hpp"

namespace kdml {
    namespace net {
        PingMessage::PingMessage(KademliaNodeInfo& src,
                                 KademliaNodeInfo& dest) :
                Message(MessageType::PING, src, dest) {}


        void PingMessage::print(std::ostream& os) const {
            os << "Ping from " << src << " to " << dest << std::endl;
        }
    }
}
