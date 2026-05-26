#pragma once
#include "syscalls/syscalls.h"
#include "fs/fs.h"
#include "custom/term.h"

static u64 sys_stub(const syscall_args_t* args) {
    panic((const char *)args->arg1);
    return 0;
}

#define SYSCALLS \
X(0,  sys_read) /* Checked */ \
X(1, sys_write) \
X(2,  sys_open) /* Checked */ \
X(3, sys_close) /* Checked */ \
X(4, sys_stat) /* Checked */\
X(5, sys_fstat) /* Checked */\
X(8, sys_lseek) \
X(16, sys_ioctl) /* Checked */ \
X(100, sys_term) /* Checked */\
X(999, sys_stub) /* Checked */\

extern syscall_fun_t syscall_table[];
extern const u64 syscallCount;
