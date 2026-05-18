#pragma once
#include "syscalls/syscalls.h"
#include "fs/fs.h"
#include "custom/term.h"

#define SYSCALLS \
X(0,  sys_read) \
X(1, sys_write) \
X(2,  sys_open) \
X(3, sys_close) \
X(100, sys_term) \

extern syscall_fun_t syscall_table[];
extern const u64 syscallCount;
