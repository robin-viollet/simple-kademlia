//
// Created by jestjest on 11/11/17.
//

#pragma once

#include <cereal/access.hpp>
#include <boost/multiprecision/cpp_int.hpp>

struct KademliaNodeInfo {
    std::string ip_addr;
    short port;
    std::string idStr;

    KademliaNodeInfo() = default;

    KademliaNodeInfo(std::string ip_addr, short port, std::string id) :
            ip_addr(std::move(ip_addr)), port(port), idStr(id) {}

    friend std::ostream& operator<<(std::ostream& os, const KademliaNodeInfo& node) {
        os << "[id: " << node.idStr;
        os << ", IP: " << node.ip_addr << ":" << node.port << "]";
        return os;
    }

    template<class Archive>
    void serialize(Archive& ar) {
        ar(ip_addr, port, idStr);
    }
};
