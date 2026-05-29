#include "common.h"
#include "rbtree.h"
#include "abi-bits/errno.h"
#include "abi-bits/vm-flags.h"

u64 do_sys_mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset) {
    vm_area_t* vma = kcalloc(1, sizeof(vm_area_t));
    vma->start = (u64)addr;
    vma->end = (u64)addr + length;
    bstree_insert(
        &current_thread->process->vma_tree,
        &vma->node
    );

    return (u64)addr;
    // return -ENOSYS;
}