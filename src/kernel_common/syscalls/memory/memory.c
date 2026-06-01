#include "common.h"
#include "rbtree.h"
#include "abi-bits/errno.h"
#include "abi-bits/vm-flags.h"

u64 do_sys_mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset) {
    terminal_write("\nMMAP| addr: ");
    terminal_write_hex_u64((uint64_t)addr);
    terminal_write(", length: ");
    terminal_write_u64(length);
    terminal_write("\nprot:");
    if (prot & PROT_NONE)
        terminal_write(" NONE");
    if (prot & PROT_READ)
        terminal_write(" READ");
    if (prot & PROT_WRITE)
        terminal_write(" READ&WRITE");
    if (prot & PROT_EXEC)
        terminal_write(" EXEC");
    terminal_write("\nflags:");
    if (flags & MAP_FILE)
        terminal_write(" FILE");
    if (flags & MAP_SHARED)
        terminal_write(" SHARED");
    if (flags & MAP_PRIVATE)
        terminal_write(" PRIVATE");
    if (flags & MAP_FIXED)
        terminal_write(" FIXED");
    if (flags & MAP_ANONYMOUS)
        terminal_write(" ANONYMOUS");
    terminal_write("\nfd: ");
    if (fd == -1) {
        terminal_write("-1");
    } else {
        terminal_write_u64(fd);
    }
    terminal_write(", offset: ");
    terminal_write_u64(offset);
    terminal_write("\n");


    if (flags & MAP_FILE)
        return -EINVAL;
    u64 len = ALIGN_UP(length, PAGE_SIZE);
    if (len == 0)
        return -EINVAL;
    // MAKE VMA FLAGS
    u64 vma_flags = 0;
    // GET FD
    file_t* file = process_unpack_fd(CUR_PROCESS, fd);
    // TODO: check addr hint

    addr = (void*)vma_find_free_region(CUR_PROCESS, length);

    return mmap_region(file, (u64)addr, len, vma_flags, offset);
}