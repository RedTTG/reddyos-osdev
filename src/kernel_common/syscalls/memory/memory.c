#include "common.h"
#include "abi-bits/errno.h"
#include "abi-bits/vm-flags.h"

u64 do_sys_mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset) {
    terminal_write("MMAP| addr: ");
    terminal_write_hex_u64((uint64_t)addr);
    terminal_write(", length: ");
    terminal_write_u64(length);
    terminal_write("\nprot:");
    if (prot & PROT_READ)
        terminal_write(" READ");
    if (prot & PROT_WRITE)
        terminal_write(" READ&WRITE");
    if (prot & PROT_EXEC)
        terminal_write(" EXEC");
    terminal_write("\nflags:");
    if (flags & MAP_SHARED)
        terminal_write(" SHARED");
    if (flags & MAP_PRIVATE)
        terminal_write(" PRIVATE");
    if (flags & MAP_ANONYMOUS)
        terminal_write(" ANONYMOUS");
    terminal_write("\nfd: ");
    terminal_write_u64(fd);
    terminal_write(", offset: ");
    terminal_write_u64(offset);
    terminal_write("\n");
    return -ENOSYS;
}