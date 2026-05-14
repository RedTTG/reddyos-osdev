#pragma once

#pragma once
#include <stdint.h>

typedef struct thread
{
    uint64_t rsp; // Important in the top for ctx switch
    uint8_t* kernel_stack;
    uint8_t* user_stack;
    uint64_t tid;
    void (*entry)(void* arg);


    struct thread* next;
} thread_t;

extern void thread_entry_kernel();
extern void thread_entry_user();
extern void arch_switch_thread(thread_t* old, thread_t* next);
thread_t* thread_create(void (*entry)(void* arg));
thread_t* user_thread_create(void (*entry)(void* arg));