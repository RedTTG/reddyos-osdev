#pragma once
#include "assert.h"
#include "bstree.h"

typedef enum {
    VMA_ANON,
    VMA_FILE
} vma_type_t;

typedef struct vm_area {
    bstree_node_t node;
    u64 start;
    u64 end;

    u64 prot;
    u64 flags;

    vma_type_t type;

    union {
        struct {
            void* file;
            u64 offset;
        };

        struct {
            u64 anon_id;
        };
    };
} vm_area_t;

static inline uint64_t vma_tree_get_value(bstree_node_t* node) {
    vm_area_t* n = CONTAINER_OF(node, vm_area_t, node);
    return n->start;
}
