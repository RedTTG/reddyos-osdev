#pragma once
#include <stdint.h>

typedef struct thread thread_t;

typedef struct process {
    address_space_t address_space;

    uint64_t entry_point;

    uint64_t user_stack_top;
    uint64_t user_stack_bottom;

    thread_t* main_thread;
    uint64_t pid;

    fd_t** fds;
    size_t fd_capacity;
} process_t;

process_t* process_create(const char* filename);
