//
// Created by jestjest on 11/30/17.
//

#ifndef SIMPLE_KADEMLIA_CALLBACKS_HPP
#define SIMPLE_KADEMLIA_CALLBACKS_HPP

#include <node/nodeinfo.hpp>

namespace kdml {
    using Nodes = std::vector<NodeInfo>;
    using GetCallback = std::function<void(Nodes)>;
    using SimpleCallback = std::function<void(bool failure)>;
}

#endif //SIMPLE_KADEMLIA_CALLBACKS_HPP
