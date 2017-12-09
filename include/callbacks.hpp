//
// Created by jestjest on 11/30/17.
//

#ifndef SIMPLE_KADEMLIA_CALLBACKS_HPP
#define SIMPLE_KADEMLIA_CALLBACKS_HPP

#include <node/nodeinfo.hpp>

namespace kdml {
    using GetResult = std::shared_ptr<kdml::NodeInfo>;
    using GetCallback = std::function<bool(std::vector<GetResult>)>;
    using SimpleCallback = std::function<void(bool failure)>;
}

#endif //SIMPLE_KADEMLIA_CALLBACKS_HPP
