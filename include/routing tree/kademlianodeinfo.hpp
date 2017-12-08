//
// Created by jestjest on 11/11/17.
//

#ifndef INC_244B_FINAL_KADEMLIANODEINFO_HPP
#define INC_244B_FINAL_KADEMLIANODEINFO_HPP

#include <ostream>

namespace kdml {
    struct KademliaNodeInfo {
        std::string ip_addr;
        short port;
        std::string idStr;

        template<class Archive>
        void serialize(Archive& ar) {
            ar(ip_addr, port, idStr);
        }

        friend std::ostream& operator<<(std::ostream& os, const KademliaNodeInfo& node) {
            os << "[id: " << node.idStr;
            os << ", IP: " << node.ip_addr << ":" << node.port << "]";
            return os;
        }
    };
}

#endif  //INC_244B_FINAL_KADEMLIANODEINFO_HPP
