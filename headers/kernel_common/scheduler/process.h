#pragma once
#include <stdint.h>

typedef struct thread thread_t;

typedef struct process {
    address_space_t address_space;

    uint64_t entry_point;

    uint64_t rsp;
    uint64_t user_stack_bottom;
    uint64_t user_stack_bottom_max;

    thread_t* main_thread;
    uint64_t pid;

    fd_t** fds;
    size_t fd_capacity;
} process_t;

process_t* process_create(const char* filename);
