#pragma once
#define IA32_EFER           0xC0000080
#define IA32_STAR           0xC0000081
#define IA32_LSTAR          0xC0000082
#define IA32_FMASK          0xC0000084
#define IA32_GSBASE         0xC0000101
#define IA32_KERNEL_GSBASE  0xC0000102

#define EFER_SCE (1 << 0)

typedef struct {
    uint64_t user_rsp;
    uint64_t user_rcx;
    uint64_t user_r11;
} percpu_data_t;

typedef struct {
    uint64_t rax;  // Syscall number / return value
    uint64_t rdi;  // Argument 1
    uint64_t rsi;  // Argument 2
    uint64_t rdx;  // Argument 3
    uint64_t r10;  // Argument 4 (R10 is used instead of RCX in syscall ABI)
    uint64_t r8;   // Argument 5
    uint64_t r9;   // Argument 6
} syscall_args_t;

extern void syscall_entry(void);
extern long syscall_handler(syscall_args_t* args);
void syscall_init(void);

// Handlers
#include "syscalls/filesystem.h"