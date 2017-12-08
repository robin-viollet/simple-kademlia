//
// Created by jestjest on 11/11/17.
//

#ifndef SIMPLE_KADEMLIA_ROUTINGTABLE_HPP
#define SIMPLE_KADEMLIA_ROUTINGTABLE_HPP

#include "kbucket.hpp"

struct RoutingTreeNode {
    kBucket* bucket;
    RoutingTreeNode* zero;
    RoutingTreeNode* one;

    RoutingTreeNode(kBucket* bucket) {

    };

    RoutingTreeNode(kBucket* bucket,
                    RoutingTreeNode* zero, RoutingTreeNode* one) {

    };
};

class RoutingTree {
    RoutingTreeNode* root;

public:

};

#endif //SIMPLE_KADEMLIA_ROUTINGTABLE_HPP
