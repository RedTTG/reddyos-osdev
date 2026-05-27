#include "common.h"
#include "abi-bits/errno.h"

u64 do_sys_mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset) {
    return -ENOSYS;
}