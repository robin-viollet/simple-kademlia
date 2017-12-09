/*
 * Created by sierrakn on 11/11/17.
 * Routing Tree contains (up to) one k-bucket for each bit of node id.
 * Each k-bucket has up to k node infos and covers section of node id space with some common prefix
 * Starts with single k-bucket covering whole id space,
 * when learns of node u if bucket is not full, just inserts it into bucket
 * If bucket is full and contains node's own id split it. If not drop new node =>
 * If node's id starts with 1, only one k-bucket devoted to id's starting with 0 (distance further)
 * After splitting contents reattempt insertion
 */

#ifndef SIMPLE_KADEMLIA_ROUTINGTABLE_HPP
#define SIMPLE_KADEMLIA_ROUTINGTABLE_HPP

#include "kbucket.hpp"


namespace kdml {
    struct RoutingTreeNode {
        kBucket *bucket;
        RoutingTreeNode *zero;
        RoutingTreeNode *one;
        RoutingTreeNode *parent;

        RoutingTreeNode(kBucket *bucket, RoutingTreeNode *parent) {
            this.bucket = bucket;
            this.parent = parent;
            this.zero = NULL;
            this.one = NULL;
        };

        RoutingTreeNode(kBucket *bucket,
                        RoutingTreeNode *zero, RoutingTreeNode *one) {
            this.bucket = bucket;
            this.zero = zero;
            this.one = one;
        };
    };

    class RoutingTree {
        RoutingTreeNode *root;
        uint256_t m_node_id;

        RoutingTreeNode *getTreeNode(RoutingTreeNode *treeNode, uint256_t node_id, uint16_t bit_index) {
            uint256_t branch = node_id & (1 << bit_index);
            if (branch) {
                if (treeNode->one == NULL) return treeNode;
                return getTreeNode(treeNode->one, node_id, bit_index-1);
            }
            if (treeNode->zero == NULL) return treeNode;
            return getTreeNode(treeNode->zero, node_id, bit_index-1);
        }

        RoutingTreeNode *getTreeNode(uint256_t node_id) {
            return getTreeNode(root, node_id, 255);
        }

        std::vector<NodeInfo*> getClosestNodes(int num, uint256_t key) {
            std::vector<NodeInfo*> closest;
            RoutingTreeNode *treeNode = getTreeNode(key);
            int num_nodes = 0;
            while (num_nodes < num && treeNode != NULL) {
                kBucket *bucket = treeNode->bucket;
                num_nodes += bucket->getNodes(closest, num-num_nodes);
                treeNode = treeNode->parent;
            }

            return closest;
        }

    public:

        RoutingTree(uint256_t node_id) {
            m_node_id = node_id;
            root = new RoutingTreeNode(new kBucket(0, 0), NULL);
        }

        // Return k nodes from tree node, and if not enough keep getting more from parent
        // Unless < k in whole tree in which case returns all nodes it knows about
        std::vector<NodeInfo*> getKClosestNodes(uint256_t key) {
            //todo: make k in kbucket global config?
            return getClosestNodes(k, key);
        }

        std::vector<NodeInfo*> getAClosestNodes(int a, uint256_t key) {
            return getClosestNodes(a, key);
        }

        bool insertNode(NodeInfo *node) {
            uint256_t node_id = node->node_id;
            RoutingTreeNode *treeNode = getTreeNode(node_id);
            kBucket *bucket = treeNode->bucket;
            bool success = bucket->insertNode(node);

            // If bucket is full and its range contains our node id, split it
            while (!success && bucket->rangeContainsId(m_node_id)) {
                kBucket *newBucket = bucket->split();

                // original bucket contains zero branch, new bucket now contains one branch
                RoutingTreeNode *zeroTreeNode = new RoutingTreeNode(bucket, treeNode);
                RoutingTreeNode *oneTreeNode = new RoutingTreeNode(newBucket, treeNode);

                treeNode->zero = zeroTreeNode;
                treeNode->one = oneTreeNode;
                treeNode->bucket = NULL;

                // try insertion again
                treeNode = getTreeNode(node_id);
                bucket = treeNode->bucket;
                success = bucket->insertNode(node);
            }
            return success;
        }

    };
}


#endif //SIMPLE_KADEMLIA_ROUTINGTABLE_HPP
