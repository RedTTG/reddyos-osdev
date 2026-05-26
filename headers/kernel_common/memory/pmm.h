#pragma once
#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE 4096

typedef struct pmm_node
{
    struct pmm_node* next;
} pmm_node_t;

void pmm_init(void);
void pmm_fill(uint64_t pages);
void* pmm_alloc_page(void);
void pmm_free_page(void* page);