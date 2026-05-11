#pragma once
#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE 4096
#define MAX_PAGES  (1024 * 1024) // adjust later

void pmm_init(void);
void* pmm_alloc_page(void);
void* pmm_alloc_virt_page(void);
void pmm_free_page(void* page);
void pmm_free_virt_page(void* page);