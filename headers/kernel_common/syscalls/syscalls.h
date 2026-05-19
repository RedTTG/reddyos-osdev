#pragma once
#define IA32_EFER           0xC0000080
#define IA32_STAR           0xC0000081
#define IA32_LSTAR          0xC0000082
#define IA32_FMASK          0xC0000084
#define IA32_GSBASE         0xC0000101
#define IA32_KERNEL_GSBASE  0xC0000102

#define EFER_SCE (1 << 0)
#define EFER_NXE (1 << 11)

typedef struct {
    u64 user_rsp;
    u64 user_rcx;
    u64 user_r11;
} percpu_data_t;

typedef struct {
    u64 rax;    // rax | Syscall number / return value
    u64 arg1;   // rdi | Argument 1
    u64 arg2;   // rsi | Argument 2
    u64 arg3;   // rdx | Argument 3
    u64 arg4;   // r10 | Argument 4 (R10 is used instead of RCX in syscall ABI)
    u64 arg5;   //  r8 | Argument 5
    u64 arg6;   //  r9 | Argument 6
} syscall_args_t;

typedef u64 (*syscall_fun_t)(const syscall_args_t* args);

extern void syscall_entry(void);
extern u64 syscall_handler(syscall_args_t* args);
void syscall_init(void);