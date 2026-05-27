#pragma once
#include "syscalls/syscalls.h"
#include "fs/fs.h"
#include "custom/term.h"
#include "memory/memory.h"

static u64 sys_stub(const syscall_args_t* args) {
    panic((const char *)args->arg1);
    return 0;
}

#define SYSCALLS        \
X(0,  sys_read)         \
X(1, sys_write)         \
X(2,  sys_open)         \
X(3, sys_close)         \
X(4, sys_stat)          \
X(5, sys_fstat)         \
X(8, sys_lseek)         \
X(9, sys_mmap)          \
X(16, sys_ioctl)        \
X(100, sys_term)        \
X(999, sys_stub)        \

extern syscall_fun_t syscall_table[];
extern const u64 syscallCount;
