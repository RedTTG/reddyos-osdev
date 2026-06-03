#pragma once
#include "assert.h"
#include "bstree.h"

#define VMA_BASE 0x10000000ULL

typedef enum {
    VMA_ANON,
    VMA_FILE
} vma_type_t;

typedef struct vm_area {
    bstree_node_t node;
    u64 start;
    u64 end;

    u64 vm_flags;

    vma_type_t type;

    union {
        struct {
            file_t* file;
            u64 pgoff;
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

vm_area_t* vma_create(
    uint64_t start,
    uint64_t length,
    uint64_t prot,
    uint64_t flags
);

void vma_destroy(vm_area_t* vma);

bool vma_insert(process_t* p, vm_area_t* vma);

void vma_remove(process_t* proc, vm_area_t* vma);

vm_area_t* vma_find(process_t* p, uint64_t addr);

bool vma_overlaps(
    process_t* p,
    uint64_t start,
    uint64_t end
);

uint64_t vma_find_free_region(
    process_t* p,
    uint64_t length
);

u64 mmap_region(file_t *file, u64 addr,
                u64 len, u64 vm_flags,
                u64 pgoff);
