#pragma once
#include <stdint.h>

#include "bstree.h"

#define USER_CODE_BASE   0x400000
#define USER_STACK_TOP   0xf0000000
#define USER_HEAP_BASE   0x80000000
#define USER_STACK_PAGES 32

typedef struct thread thread_t;

typedef struct __attribute__((aligned(16))) process {
    address_space_t address_space;

    uint64_t entry_point;

    uint64_t rsp;
    uint64_t user_stack_bottom;
    uint64_t user_stack_bottom_max;

    thread_t* main_thread;
    uint64_t pid;

    fd_t** fds;
    size_t fd_capacity;

    bstree_t vma_tree;
} process_t;

process_t* process_create(const char* filename);
bool grow_user_stack(process_t* p);
