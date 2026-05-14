#pragma once
#include <stdint.h>

typedef struct thread thread_t;

typedef struct process {
    uint64_t cr3;

    uint64_t entry_point;

    uint64_t user_stack_top;
    uint64_t user_stack_bottom;

    thread_t* main_thread;
    uint64_t pid;
} process_t;

process_t* process_create(const char* filename);
