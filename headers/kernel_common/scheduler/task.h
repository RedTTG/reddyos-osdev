#pragma once

#pragma once
#include <stdint.h>

typedef struct context
{
    uint64_t r15, r14, r13, r12;
    uint64_t rbx, rbp, rsp, rax;
} context_t;

typedef struct task
{
    context_t context;

    void (*entry)(void);
    uint8_t* stack;
    uint64_t id;

    struct task* next;
} task_t;

task_t* task_create(void (*entry)(void));