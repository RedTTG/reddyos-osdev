#pragma once

#include "stub_lists.h"

typedef struct
{
    uint64_t r15, r14, r13, r12;
    uint64_t r11, r10, r9, r8;
    uint64_t rsi, rdi, rbp, rdx;
    uint64_t rcx, rbx, rax;

    uint64_t interrupt_number;
    uint64_t error_code;

    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
} interrupt_frame_t;

#define DECLARE_ISR_STUB(n) extern char isr_stub_##n[];
ISR_STUB_LIST(DECLARE_ISR_STUB)
#undef DECLARE_ISR_STUB
