//
// Created by jestjest on 12/2/17.
//

#ifndef SIMPLE_KADEMLIA_QUERYMESSAGE_HPP
#define SIMPLE_KADEMLIA_QUERYMESSAGE_HPP

#include "message.hpp"


namespace kdml {
    namespace net {

        class QueryMessage : public Message {
        protected:
            std::array<unsigned char, 32> id{};
            QueryType qtype{};
        public:
            explicit QueryMessage(std::array<unsigned char, 32> id,
                                  uint16_t tid, QueryType type)
                    : Message(tid, MessageType::QUERY),
                      id(std::move(id)), qtype(type) {}

            QueryType getQueryType() const { return qtype; }

            template <class Archive>
            void serialize(Archive& ar) {
                ar(cereal::base_class<Message>(this), qtype, id);
            }
        };

    }
}

#endif //SIMPLE_KADEMLIA_QUERYMESSAGE_HPP
