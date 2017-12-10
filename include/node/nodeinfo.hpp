//
// Created by jestjest on 11/11/17.
//

#ifndef SIMPLE_KADEMLIA_NODEINFO_HPP
#define SIMPLE_KADEMLIA_NODEINFO_HPP

// Boost earlier 1.60 doesn't have import_bits/export_bits
#if BOOST_VERSION < 106000
#include "aux/cppint_import_export.hpp"
#endif

#include <ostream>
#include <boost/multiprecision/cpp_int.hpp>
#include <cereal/types/vector.hpp>
#include <aux/picosha2.hpp>
#include <arpa/inet.h>

namespace kdml {

    namespace mp = boost::multiprecision;

    struct NodeInfo {

        unsigned long ipAddr{};
        unsigned short port{0};
        mp::uint256_t id{};

        NodeInfo() = default;

        NodeInfo(const std::string& ip, unsigned short p)
                : port(p) {

            inet_pton(AF_INET, ip.c_str(), &ipAddr);
            std::vector<unsigned char> hash(32);
            picosha2::hash256(ip.begin(), ip.end(), hash.begin(), hash.end());
            mp::import_bits(id, hash.begin(), hash.end());
        }

        template<class Archive>
        void save(Archive& ar) const {
            std::vector<unsigned char> idVec(32);
            mp::export_bits(id, std::back_inserter(idVec), 8);
            ar(ipAddr, port, idVec);
        }

        template<class Archive>
        void load(Archive& ar) {
            std::vector<unsigned char> idVec(32);
            ar(ipAddr, port, idVec);
            mp::import_bits(id, idVec.begin(), idVec.end());
        }

        friend std::ostream& operator<<(std::ostream& os, const NodeInfo& node) {
            os << "(id: " << node.id
               << ", IP: " << node.getIpAddr()
               << ":" << node.port << ")";
            return os;
        }

        std::string getIpAddr() const {
            char str[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(ipAddr), str, INET_ADDRSTRLEN);
            std::string addrStr(str, INET_ADDRSTRLEN);
            return addrStr;
        }
    };
}

#endif //SIMPLE_KADEMLIA_NODEINFO_HPP
