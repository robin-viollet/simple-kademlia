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
#include <cereal/types/array.hpp>
#include <aux/picosha2.hpp>
#include <arpa/inet.h>

namespace kdml {

    namespace mp = boost::multiprecision;

    struct NodeInfo {
        unsigned long ipAddr{};
        unsigned short port{0};
        std::array<unsigned char, 32> id{};
        mp::uint256_t idNum;

        NodeInfo() = default;

        NodeInfo(const std::string& ip, unsigned short p)
                : port(p) {

            inet_pton(AF_INET, ip.c_str(), &ipAddr);
            picosha2::hash256(ip.begin(), ip.end(), id.begin(), id.end());
            mp::import_bits(idNum, id.begin(), id.end());
        }

        template<class Archive>
        void save(Archive& ar) const {
            ar(ipAddr, port, id);
        }

        template<class Archive>
        void load(Archive& ar) {
            ar(ipAddr, port, id);
            mp::import_bits(idNum, id.begin(), id.end());
        }

        friend std::ostream& operator<<(std::ostream& os, const NodeInfo& node) {
            char str[INET_ADDRSTRLEN];

            os << "[id: " << node.idNum
               << ", IP: " << node.getIpAddr()
               << ":" << node.port << "]" << std::endl;
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
