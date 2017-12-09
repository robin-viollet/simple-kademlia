/*
 * Created by sierrakn on 11/11/17.
 * Contains up to k node infos.
 * Kept sorted by last time seen - least recently seen at head, most at tail
 */

#ifndef SIMPLE_KADEMLIA_KBUCKET_HPP
#define SIMPLE_KADEMLIA_KBUCKET_HPP

#include <list>
#include "node/nodeinfo.hpp"

namespace kdml {

    const int k = 20;

    class kBucket {

        std::list<NodeInfo*> contacts;
        uint256_t prefix;
        uint16_t tree_level;

    public:

        kBucket(uint256_t prefix, uint16_t tree_level) {
            this->prefix = prefix;
            this->tree_level = tree_level;
        }

        bool insertNode(NodeInfo *node) {
            if (contacts.size() < k) {
                contacts.push_back(node);
                return true;
            }
            return false;
        }

        // Checks if first tree_level bits of prefix and node_id are the same
        bool rangeContainsId(uint256_t node_id) {
            return (prefix >> 256 - tree_level) ^ (node_id >> 256 - tree_level) == 0;
        }

        // Iterates over node info list and
        void splitNodes(kBucket *new_bucket) {
            std::list<item*>::iterator i = contacts.begin();
            while (i != contacts.end()) {
                NodeInfo *node = *i;
                if (new_bucket->rangeContainsId(node->node_id)) {
                    new_bucket->insertNode(node);
                    i = items.erase(i);
                } else {
                    i++;
                }
            }
        }

        // Splits bucket and returns one branch bucket.
        // Splits contents between new buckets
        // todo: check if tree level is max (probably won't happen)
        kBucket *split() {
            tree_level++;
            uint256_t new_prefix = prefix | (1 << 256 - tree_level);
            kBucket *new_bucket = new kBucket(new_prefix, tree_level);
            splitNodes(new_bucket);
            return new_bucket;
        }

        // adds num_nodes to vector or if has less than num_nodes,
        // adds all contacts to vector.
        // Returns how many nodes added.
        int getNodes(std::vector<NodeInfo*>& nodes, int num_nodes) {
            int num_added = 0;
            std::list<item*>::iterator i = contacts.begin();
            for (i; i != contacts.end() && num_added < num_nodes; i++) {
                NodeInfo *node = *i;
                nodes.push_back(node);
                num_added++;
            }
            return num_added;
        }
    };
}

#endif //SIMPLE_KADEMLIA_KBUCKET_HPP
