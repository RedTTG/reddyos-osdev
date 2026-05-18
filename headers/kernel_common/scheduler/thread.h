#pragma once

#pragma once
#include <stdint.h>

typedef struct process process_t;

typedef struct thread
{
    uint64_t rsp; // Important in the top for ctx switch
    uint8_t* kernel_stack;
    uint8_t* user_stack;
    uint64_t tid;
    void (*entry)(void* arg);

    process_t* process;

    struct thread* next;

    uint8_t fpu_state[512] __attribute__((aligned(16)));
} thread_t;

inline bool is_process_thread(const thread_t* thread) {
    return thread->process != 0;
}

extern void thread_entry_kernel();
extern void thread_entry_user();
extern void arch_switch_thread(thread_t* old, thread_t* next);
thread_t* thread_create(void (*entry)(void* arg));
thread_t* user_thread_create(process_t* process);
