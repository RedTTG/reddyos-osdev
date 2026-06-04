#pragma once
#include "syscalls.h"
#include "fs/fs.h"
#include "custom/term.h"
#include "memory/memory.h"
#include "arch/arch.h"
#include "time/clock.h"
#include "time/sleep.h"

static u64 sys_stub(const syscall_args_t* args) {
    panic((const char *)args->arg1);
    return 0;
}

#define SYSCALLS            \
X(0,  sys_read)             \
X(1, sys_write)             \
X(2,  sys_open)             \
X(3, sys_close)             \
X(4, sys_stat)              \
X(5, sys_fstat)             \
X(8, sys_lseek)             \
X(9, sys_mmap)              \
X(11, sys_munmap)           \
X(16, sys_ioctl)            \
X(35, sys_nanosleep)        \
X(100, sys_term)            \
X(158, sys_arch_prctl)      \
X(228, sys_clock_gettime)   \
X(999, sys_stub)            \

extern syscall_fun_t syscall_table[];
extern const u64 syscallCount;
