#pragma once
#include "assert.h"
#include "bstree.h"

typedef enum {
    VMA_ANON,
    VMA_FILE
} vma_type_t;

typedef struct vm_area {
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

    struct vm_area* next;
} vm_area_t;

typedef struct vm_area_node {
    bstree_node_t node;
    vm_area_t area;
} vm_area_node_t;

static uint64_t vma_tree_get_value(bstree_node_t* node) {
    vm_area_node_t* n = CONTAINER_OF(node, vm_area_node_t, node);
    return n->area.start;
}
