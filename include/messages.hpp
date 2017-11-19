/*
 * Message types and objects sent between nodes
 */

#pragma once

#include "kademlianodeinfo.hpp"
#include "cereal/types/polymorphic.hpp"

namespace kdml {
    namespace net {

        enum class MessageType {
            PING,
            REPLY,
            FIND_NODE,
            FIND_VALUE,
            STORE,
            NONE
        };

        class Message {
        protected:
            MessageType type;
            KademliaNodeInfo src;
            KademliaNodeInfo dest;
        public:
            Message(MessageType type,
                    KademliaNodeInfo src, KademliaNodeInfo dest) :
                    type(type), src(std::move(src)), dest(std::move(dest)) {}

            ~Message() = default;

            MessageType getType() { return type; }
            virtual void printContents() = 0;
        };

        class PingMessage : public Message {
        public:
            PingMessage(KademliaNodeInfo& src, KademliaNodeInfo& dest) :
                    Message(MessageType::PING, src, dest) {}

            void printContents() override {
                std::cout << "Ping from " << src << " to " << dest << std::endl;
            }

            template<class Archive>
            void serialize(Archive& ar) {
                ar(src, dest);
            }

            template <class Archive>
            static void load_and_construct(Archive& ar, cereal::construct<PingMessage>& construct) {
                KademliaNodeInfo src;
                KademliaNodeInfo dest;
                ar(src, dest);
                construct(src, dest);
            }

        };

    }
}

CEREAL_REGISTER_TYPE(kdml::net::PingMessage);
CEREAL_REGISTER_POLYMORPHIC_RELATION(kdml::net::Message, kdml::net::PingMessage);
