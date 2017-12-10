//
// Created by jestjest on 11/19/17.
//

#ifndef SIMPLE_KADEMLIA_FINDNODERESPONSE_HPP
#define SIMPLE_KADEMLIA_FINDNODERESPONSE_HPP

#include "responseMessage.hpp"
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>
#include <callbacks.hpp>
#include <utility>

namespace kdml {
    namespace net {

        class FindNodeResponse : public ResponseMessage {

        protected:
            Nodes nodes;

        public:
            FindNodeResponse(mp::uint256_t id, uint32_t tid, Nodes nodes)
                    : ResponseMessage(std::move(id), tid, QueryType::FIND_NODE),
                      nodes(std::move(nodes)) {}

            void print(std::ostream& os) const override {
                os << "[" << mtype << "][" << qtype << "][TID="
                   << tid << "][nodeID=" << id << "][nodes=";

                for (size_t i = 0; i < nodes.size(); i++) {
                    os << nodes[i];
                    if (i != nodes.size() - 1) {
                        os << ", ";
                    }
                }

                os << "]";
            }

            template<class Archive>
            void serialize(Archive& ar) {
                ar(cereal::base_class<ResponseMessage>(this), nodes);
            }

            template<class Archive>
            static void load_and_construct(Archive& ar, cereal::construct<FindNodeResponse>& construct) {
                uint32_t tid{};
                MessageType mtype{};
                QueryType qtype{};
                std::vector<unsigned char> idVec(32);
                mp::uint256_t id;
                Nodes nodes;

                ar(tid, mtype, qtype, idVec, nodes);
                mp::import_bits(id, idVec.begin(), idVec.end());
                construct(id, tid, nodes);
            }
        };
    }
}

CEREAL_REGISTER_TYPE(kdml::net::FindNodeResponse);

#endif SIMPLE_KADEMLIA_FINDNODERESPONSE_HPP
