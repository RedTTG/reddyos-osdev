#include "common.h"
#include "abi-bits/errno.h"

void vma_add_page(vm_area_t *vma, u64 start, u64 phys) {
    vma_phys_page_t *new_page = kmalloc(sizeof(vma_phys_page_t));
    new_page->start = start;
    new_page->phys = phys;
    new_page->next = NULL;

    vma_phys_page_t* page = vma->phys_pages;
    if (!page) {
        vma->phys_pages = new_page;
        return;
    }
    while (page->next) {
        page = page->next;
    }
    page->next = new_page;
}

void vma_cleanup(vm_area_t *vma) {
    vma_phys_page_t* page = vma->phys_pages;
    vma_phys_page_t* prev;
    if (!page) {
        return;
    }
    while (page) {
        if (page->start < vma->start || page->start >= vma->end) {
            // This page is outside the VMA range, we should free it
            terminal_write("Cleaning up page at ");
            terminal_write_hex_u64(page->start);
            terminal_write(" which is outside the VMA range [");
            terminal_write_hex_u64(vma->start);
            terminal_write(", ");
            terminal_write_hex_u64(vma->end);
            terminal_write(")\n");
            vmm_unmap(&CUR_PROCESS->address_space, page->start);
            // Delete the entry
            if (prev) {
                prev->next = page->next;
            } else {
                vma->phys_pages = page->next;
            }
            kfree(page);
        } else {
            prev = page;
            page = page->next;
        }

        if (!page) {
            if (prev) {
                page = prev->next;
            }
            else {
                page = vma->phys_pages;
            }
        }

    }
}

void vma_destroy(vm_area_t *vma) {
    vma_phys_page_t* page = vma->phys_pages;
    while (page) {
        terminal_write("Destroying page at ");
        terminal_write_hex_u64(page->start);
        terminal_write(" which is in the VMA range [");
        terminal_write_hex_u64(vma->start);
        terminal_write(", ");
        terminal_write_hex_u64(vma->end);
        terminal_write(")\n");
        vmm_unmap(&CUR_PROCESS->address_space, page->start);
        vma_phys_page_t* next = page->next;
        kfree(page);
        page = next;
    }
}

bool vma_insert(process_t *p, vm_area_t *vma) {
    return bstree_insert(&p->vma_tree, &vma->node) != NULL;
}

void vma_remove(process_t *proc, vm_area_t *vma) {
    bstree_remove(&proc->vma_tree, &vma->node);
}

vm_area_t *vma_find(process_t *p, uint64_t addr) {
    bstree_node_t *node =
            bstree_search(
                &p->vma_tree,
                addr,
                BST_SEARCH_TYPE_NEAREST_LTE
            );

    if (!node)
        return NULL;

    vm_area_t *vma =
            CONTAINER_OF(node, vm_area_t, node);

    if (addr >= vma->start && addr < vma->end) // It's important to be exclusive of the end
        return vma;

    return NULL;
}

bool vma_overlaps(process_t *p, uint64_t start, uint64_t end) {
    if (!p->vma_tree.root)
        return false;
    bstree_node_t *node =
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
        vm_area_t *vma =
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

static void scan(bstree_node_t *node, vm_area_t **prev, uint64_t *last_end, uint64_t length, uint64_t *result) {
    if (!node || *result) return;

    scan(node->left, prev, last_end, length, result);

    vm_area_t *vma = CONTAINER_OF(node, vm_area_t, node);

    if (vma->start - *last_end >= length) {
        *result = *last_end;
        return;
    }

    *last_end = vma->end;

    scan(node->right, prev, last_end, length, result);
}

uint64_t vma_find_free_region(process_t *p, uint64_t length) {
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
    vm_area_t *vma = kcalloc(1, sizeof(vm_area_t));
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

u64 munmap_region(u64 addr, u64 len) {
    vm_area_t *vma = vma_find(CUR_PROCESS, addr);
    if (!vma)
        return -EINVAL;

    if (addr < vma->start || addr + len > vma->end)
        return -EINVAL;

    // If the unmap region is in the middle of the VMA, we need to split it
    if (addr > vma->start && addr + len < vma->end) {
        vm_area_t *new_vma = kcalloc(1, sizeof(vm_area_t));
        new_vma->start = addr + len;
        new_vma->end = vma->end;
        new_vma->vm_flags = vma->vm_flags;
        new_vma->type = vma->type;
        if (vma->type == VMA_FILE) {
            new_vma->file = vma->file;
            new_vma->pgoff = vma->pgoff + (new_vma->start - vma->start);
        }
        vma_insert(CUR_PROCESS, new_vma);
    }

    // If the unmap region is at the start of the VMA, we just need to move the start
    if (addr == vma->start) {
        vma->start += len;
        if (vma->type == VMA_FILE) {
            vma->pgoff += len;
        }
    } else {
        // If the unmap region is at the end of the VMA, we just need to move the end
        vma->end = addr;
    }

    // If the VMA is now empty, we can remove it
    if (vma->start >= vma->end) {
        vma_remove(CUR_PROCESS, vma);
        vma_destroy(vma);
        kfree(vma);
    } else {
        vma_cleanup(vma);
    }

    return 0;
}

bool vma_fault_handler(vm_area_t *vma, u64 addr) {
    // VMA ANON
    if (vma->type == VMA_ANON) {
        void *phys_page = pmm_alloc_page();
        if (!phys_page)
            return false;
        // TODO: Adjust the actual flags
        u64 virt = ALIGN_DOWN(addr, PAGE_SIZE);
        vmm_map(&CUR_PROCESS->address_space, virt, (u64) phys_page,
                PAGE_PRESENT | PAGE_USER | PAGE_WRITABLE);
        vma_add_page(vma, virt, (u64) phys_page);
        memset(VIRT(phys_page), 0, PAGE_SIZE);
        return true;
    }
    // VMA FILE
    if (vma->file->vnode->ops->vma_fault(vma->file->vnode, vma, ALIGN_DOWN(addr, PAGE_SIZE))) {
        return true;
    }
    return false;
}
