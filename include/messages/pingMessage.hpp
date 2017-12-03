//
// Created by jestjest on 11/19/17.
//

#ifndef SIMPLE_KADEMLIA_PINGMESSAGE_HPP
#define SIMPLE_KADEMLIA_PINGMESSAGE_HPP

#include "queryMessage.hpp"
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>

namespace kdml {
    namespace net {

        class PingMessage : public QueryMessage {

        public:
            PingMessage(std::array<unsigned char, 32> id, uint16_t tid)
                    : QueryMessage(id, tid, QueryType::PING) {}

            void print(std::ostream& os) const override {
                boost::multiprecision::uint256_t idNum;
                boost::multiprecision::import_bits(idNum, id.begin(), id.end());
                os << "[" << mtype << "][" << qtype << "][TID="
                   << tid << "][nodeID=" << idNum << "]" << std::endl;
            }

            template<class Archive>
            void serialize(Archive& ar) {
                ar(cereal::base_class<QueryMessage>(this));
            }

            template<class Archive>
            static void load_and_construct(Archive& ar, cereal::construct<PingMessage>& construct) {
                std::array<unsigned char, 32> id;
                uint16_t tid;
                MessageType mtype;
                QueryType qtype;

                ar(tid, mtype, qtype, id);
                construct(id, tid);
            }
        };
    }
}

CEREAL_REGISTER_TYPE(kdml::net::PingMessage);

#endif //SIMPLE_KADEMLIA_PINGMESSAGE_HPP
