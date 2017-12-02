//
// Created by jestjest on 11/11/17.
//

#ifndef SIMPLE_KADEMLIA_KBUCKET_HPP
#define SIMPLE_KADEMLIA_KBUCKET_HPP

#include <list>
#include "node/kademlianodeinfo.hpp"

namespace kdml {
    class kBucket {
        std::list<KademliaNodeInfo> contacts;

    public:
        bool insertNode(std::string key);

        kBucket *split();
    };
}

#endif //SIMPLE_KADEMLIA_KBUCKET_HPP
