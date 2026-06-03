#include "common.h"
#include "abi-bits/errno.h"


bool vma_insert(process_t *p, vm_area_t *vma) {
    return bstree_insert(&p->vma_tree, &vma->node) != NULL;
}

void vma_remove(process_t *proc, vm_area_t *vma) {
    bstree_remove(&proc->vma_tree, &vma->node);
}

vm_area_t * vma_find(process_t *p, uint64_t addr) {
    bstree_node_t* node =
        bstree_search(
            &p->vma_tree,
            addr,
            BST_SEARCH_TYPE_NEAREST_LTE
        );

    if (!node)
        return NULL;

    vm_area_t* vma =
        CONTAINER_OF(node, vm_area_t, node);

    if (addr >= vma->start && addr < vma->end) // It's important to be exclusive of the end
        return vma;

    return NULL;
}

bool vma_overlaps(process_t *p, uint64_t start, uint64_t end) {
    if (!p->vma_tree.root)
        return false;
    bstree_node_t* node =
        bstree_search(
            &p->vma_tree,
            start,
            BST_SEARCH_TYPE_NEAREST_LTE
        );

    if (!node)
        node = bstree_minimum(p->vma_tree.root);

    if (!node)
        return false;

    while (node) {
        vm_area_t* vma =
            CONTAINER_OF(node, vm_area_t, node);

        if (vma->start >= end)
            break;

        if (start < vma->end &&
            end > vma->start)
            return true;

        node = bstree_successor(node);
    }

    return false;
}

static void scan(bstree_node_t* node, vm_area_t** prev, uint64_t *last_end, uint64_t length, uint64_t *result)
{
    if (!node || *result) return;

    scan(node->left, prev, last_end, length, result);

    vm_area_t* vma = CONTAINER_OF(node, vm_area_t, node);

    if (vma->start - *last_end >= length) {
        *result = *last_end;
        return;
    }

    *last_end = vma->end;

    scan(node->right, prev, last_end, length, result);
}

uint64_t vma_find_free_region(process_t *p, uint64_t length)
{
    uint64_t result = 0;
    uint64_t last_end = VMA_BASE;

    scan(p->vma_tree.root, NULL, &last_end, length, &result);

    if (vma_overlaps(p, last_end, last_end + length)) {
        return 0; // No suitable region found
    }

    return result ? result : last_end;
}

u64 mmap_region(file_t *file, u64 addr,
                u64 len, u64 vm_flags,
                u64 pgoff) {
    vm_area_t* vma = kcalloc(1, sizeof(vm_area_t));
    vma->start = addr;
    vma->end = addr + len;
    vma->vm_flags = vm_flags;
    u64 ret;
    if (file) {
        if (!file->vnode->ops->vma_fault) {
            ret = -ENODEV;
            goto delete;
        }
        vma->type = VMA_FILE;
        vma->file = file;
        vma->pgoff = pgoff;
    } else {
        vma->type = VMA_ANON;
    }

    vma_insert(CUR_PROCESS, vma);

    ret = vma->start;
    return ret;
delete:
    kfree(vma);
    return ret;
}
