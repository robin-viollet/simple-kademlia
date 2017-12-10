//
// Created by jestjest on 11/19/17.
//

#ifndef SIMPLE_KADEMLIA_FINDVALUEQUERY_HPP
#define SIMPLE_KADEMLIA_FINDVALUEQUERY_HPP

#include "queryMessage.hpp"
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>
#include <utility>

namespace kdml {
    namespace net {

        class FindValueQuery : public QueryMessage {
        protected:
            mp::uint256_t targetId;
        public:
            FindValueQuery(mp::uint256_t id, uint32_t tid, mp::uint256_t target)
                    : QueryMessage(std::move(id), tid, QueryType::FIND_VALUE),
                      targetId(std::move(target)) {}

            void print(std::ostream& os) const override {
                os << "[" << mtype << "][" << qtype << "][TID="
                   << tid << "][queryingID=" << id << "][target="
                   << targetId << "]";
            }

            template<class Archive>
            void save(Archive& ar) {
                std::vector<unsigned char> targetVec(32);
                mp::export_bits(targetId, std::back_inserter(targetVec), 8);
                ar(cereal::base_class<QueryMessage>(this), targetVec);
            }

            template<class Archive>
            void load(Archive& ar) {
                std::vector<unsigned char> targetVec(32);
                ar(cereal::base_class<QueryMessage>(this), targetVec);
                mp::import_bits(targetId, targetVec.begin(), targetVec.end());
            }

            template<class Archive>
            static void load_and_construct(Archive& ar, cereal::construct<FindValueQuery>& construct) {
                uint32_t tid{};
                MessageType mtype{};
                QueryType qtype{};
                std::vector<unsigned char> idVec(32);
                mp::uint256_t id;
                std::vector<unsigned char> targetVec(32);
                mp::uint256_t target;

                ar(tid, mtype, qtype, idVec, targetVec);
                mp::import_bits(id, idVec.begin(), idVec.end());
                mp::import_bits(target, targetVec.begin(), targetVec.end());
                construct(id, tid, target);
            }
        };
    }
}

CEREAL_REGISTER_TYPE(kdml::net::FindValueQuery);

#endif SIMPLE_KADEMLIA_FINDVALUEQUERY_HPP
