//
// Created by jestjest on 11/11/17.
//

#ifndef SIMPLE_KADEMLIA_KADEMLIANODEINFO_HPP
#define SIMPLE_KADEMLIA_KADEMLIANODEINFO_HPP

#include <ostream>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/random.hpp>
#include <boost/multiprecision/gmp.hpp>

using namespace boost::random;

namespace kdml {

    // TODO Hash the IP address instead?
    typedef independent_bits_engine<mt19937, 256, boost::multiprecision::uint256_t> generator_type;

    struct KademliaNodeInfo {
        std::string ipAddr;
        unsigned short port{0};
        boost::multiprecision::uint256_t id;

        KademliaNodeInfo() = default;

        KademliaNodeInfo(const std::string& ip, unsigned short p)
                : ipAddr(std::move(ip)), port(p) {
            // generate 256-bit random ID
            generator_type id_generator;
            id = id_generator();
        }

        template<class Archive>
        void save(Archive& ar) const {
            ar(ipAddr, port, id.str());
        }

        template<class Archive>
        void load(Archive& ar) {
            std::string idStr;
            ar(ipAddr, port, idStr);
            id.assign(idStr);
        }

        friend std::ostream& operator<<(std::ostream& os, const KademliaNodeInfo& node) {
            os << "[id: " << node.id;
            os << ", IP: " << node.ipAddr << ":" << node.port << "]";
            return os;
        }
    };
}

#endif //SIMPLE_KADEMLIA_KADEMLIANODEINFO_HPP
