//
// Created by aislam6 on 11/22/19.
//

#ifndef YCSB_STOREDSBENCH_BPLUSTREE_COMMON_H
#define YCSB_STOREDSBENCH_BPLUSTREE_COMMON_H

#include <libpmemobj.h>

namespace ycsbc {
    /*
     * Data Structure Section
     */

    /* minimum degree - every node (except root) must contain (BPLUSTREE_MIN_DEGREE - 1) keys */
    /* all nodes (including root) may contain at most (2*BPLUSTREE_MIN_DEGREE - 1) keys */
    #define BPLUSTREE_MIN_DEGREE 10
    #define BPLUSTREE_DEGREE 64
    /* maximum keys a node can hold */
    #define BPLUSTREE_MAX_KEYS (2 * BPLUSTREE_MIN_DEGREE - 1)

    /* maximum children a node can have */
    #define BPLUSTREE_MAX_CHILDREN (2 * BPLUSTREE_MIN_DEGREE)

    /* default length for value */
    #define DEFAULT_VALUE_LEN 128

    /* size of the pmem object pool -- 1 GB */
    #define PMEM_BPLUSTREE_POOL_SIZE ((size_t) ((size_t) 1 << 31))

    /* name of layout in the pool */
    #define BPLUSTREE_LAYOUT_NAME "bplustree_layout"

    #define LEAF_NODE_TRUE_FLAG 1
    #define LEAF_NODE_FALSE_FLAG 0

    /* types of allocations */
    enum bplustree_alloc_types {
        BPLUSTREE_NODE_TYPE,
        BPLUSTREE_ENTRY_TYPE,

        BPLUSTREE_MAX_TYPES
    };

    /* declaration of data-structures */

    /*structure size: 136 Bytes*/
    struct bplustree_entry {
        uint64_t key;   //8 Bytes
        char value[DEFAULT_VALUE_LEN];  //128 Bytes
    } __attribute__ ((aligned (8)));

    /*structure size: {24 + (144 * BPLUSTREE_MAX_KEYS) + 8} Bytes*/
    struct bplustree_dram_node {
        //flag to check if the node is leaf or not
        int is_leaf;    //4 Bytes

        //current number of keys
        int nk; //4 Bytes

        //array of <key-value> entries
        struct bplustree_entry entries[BPLUSTREE_MAX_KEYS];     //(136 * BPLUSTREE_MAX_KEYS) Bytes

        //array of child pointer
        struct bplustree_dram_node *children[BPLUSTREE_MAX_CHILDREN];   //(8 * BPLUSTREE_MAX_CHILDREN) Bytes

        //used for leaf type nodes
        struct bplustree_dram_node *next;   //8 Bytes
        struct bplustree_dram_node *previous;   //8 Bytes
    };

    struct bplustree_pmem_root {
        PMEMoid root_node_oid;
    };

    struct bplustree_pmem_root_concurrent_lock {
        PMEMoid root_node_oid;
        PMEMrwlock rwlock;
    };

    struct bplustree_pmem_root_concurrent_mlock {
        PMEMoid root_node_oid;
        PMEMmutex mlock;
    };

    /*structure size: {40 + (152 * BPLUSTREE_MAX_KEYS) + 16} Bytes*/
    struct bplustree_pmem_node {
        //flag to check if the node is leaf or not
        int is_leaf;    //4 Bytes

        //current number of keys
        int nk; //4 Bytes

        //array of <key-value> entries
        struct bplustree_entry entries[BPLUSTREE_MAX_KEYS]; //(136 * BPLUSTREE_MAX_KEYS) Bytes

        //array of child pointer
        PMEMoid children[BPLUSTREE_MAX_CHILDREN];   //(16 * BPLUSTREE_MAX_CHILDREN) Bytes

        //used for leaf type nodes
        PMEMoid next;   //16 Bytes
        PMEMoid previous;   //16 Bytes
    };
}   //ycsbc

#endif //YCSB_STOREDSBENCH_BPLUSTREE_COMMON_H
