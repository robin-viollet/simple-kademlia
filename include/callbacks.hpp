//
// Created by jestjest on 11/30/17.
//

#ifndef SIMPLE_KADEMLIA_CALLBACKS_HPP
#define SIMPLE_KADEMLIA_CALLBACKS_HPP

#include <node/kademlianodeinfo.hpp>

namespace kdml {
    using GetResult = std::shared_ptr<kdml::KademliaNodeInfo>;
    using GetCallback = std::function<bool(std::vector<GetResult>)>;
}

#endif //SIMPLE_KADEMLIA_CALLBACKS_HPP
