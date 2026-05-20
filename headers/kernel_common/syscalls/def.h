// ReSharper disable CppUnusedIncludeDirective
#pragma once
#include "syscalls/syscalls.h"

// Below are imports for syscall definition headers
#include "fs/fs.h"
#include "arch/arch.h"
#include "custom/term.h"

#define SYSCALLS \
X(0,  sys_read) /* Checked */ \
X(1, sys_write) /* Checked */ \
X(2,  sys_open) /* Checked */ \
X(3, sys_close) /* Checked */ \
X(4, sys_stat) /* Checked */ \
X(5, sys_fstat) /* Checked */ \
X(8, sys_lseek) \
X(16, sys_ioctl) /* Checked */ \
X(100, sys_term) /* Checked */ \
X(158, sys_arch_prctl) \

extern syscall_fun_t syscall_table[];
extern const u64 syscallCount;
