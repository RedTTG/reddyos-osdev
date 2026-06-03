#include "common.h"
#include "rbtree.h"
#include "abi-bits/errno.h"
#include "abi-bits/vm-flags.h"

u64 do_sys_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
    // terminal_write("\nMMAP|");
    // if (addr) {
    //     terminal_write(" hint: ");
    //     terminal_write_hex_u64((uint64_t)addr);
    // } else {
    //     terminal_write(" no hint given");
    // }
    // terminal_write(", length: ");
    // terminal_write_u64(length);
    // terminal_write("\nprot:");
    // if (prot & PROT_NONE)
    //     terminal_write(" NONE");
    // if (prot & PROT_READ)
    //     terminal_write(" READ");
    // if (prot & PROT_WRITE)
    //     terminal_write(" WRITE");
    // if (prot & PROT_EXEC)
    //     terminal_write(" EXEC");
    // terminal_write("\nflags:");
    // if (flags & MAP_FILE || fd >= 0)
    //     terminal_write(" FILE");
    // if (flags & MAP_SHARED)
    //     terminal_write(" SHARED");
    // if (flags & MAP_PRIVATE)
    //     terminal_write(" PRIVATE");
    // if (flags & MAP_FIXED)
    //     terminal_write(" FIXED");
    // if (flags & MAP_ANONYMOUS)
    //     terminal_write(" ANONYMOUS");
    // if (fd >= 0) {
    //     terminal_write("\nfd: ");
    //     if (fd == -1) {
    //         terminal_write("-1");
    //     } else {
    //         terminal_write_u64(fd);
    //     }
    //     terminal_write(", offset: ");
    //     terminal_write_u64(offset);
    // }
    // terminal_write("\n");


    u64 len = ALIGN_UP(length, PAGE_SIZE);
    if (len == 0)
        return -EINVAL;
    // MAKE VMA FLAGS
    u64 vma_flags = 0;
    // GET FD
    file_t* file = process_unpack_fd(CUR_PROCESS, fd);
    // TODO: check addr hint

    addr = (void*)vma_find_free_region(CUR_PROCESS, length);
    if (!addr) {
        terminal_write("No suitable region found for mmap\n");
        return -ENOMEM;
    }

    // terminal_write("start: ");
    // terminal_write_hex_u64((u64)addr);
    // terminal_write(" -> end: ");
    // terminal_write_hex_u64((u64)addr + length);
    // terminal_write("\n");

    return mmap_region(file, (u64)addr, len, vma_flags, offset);
}
