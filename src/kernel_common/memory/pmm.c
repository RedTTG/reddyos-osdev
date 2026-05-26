#include "common.h"

pmm_node_t* freelist_pmm_head = NULL;
u64 freelist_pmm_fill_entry = 0;
u64 freelist_pmm_fill_offset = 0;

/* Credit for the freelist pmm allocator
 SPDX-License-Identifier: MIT
 Copyright (c) 2026 Evalyn Goemer & EvalynOS Contributors
*/


void *pmm_alloc_page(void) {
    // LOCKS-TODO: later lock here
    
    if (freelist_pmm_head == NULL) pmm_fill(512);
    if (freelist_pmm_head == NULL) panic("PMM: out of memory");

    pmm_node_t* node = freelist_pmm_head;
    freelist_pmm_head = node->next;
    u64 phys = memphys((u64)node);

    // LOCKS-TODO: later unlock here

    memset(node, 0, PAGE_SIZE);
    return (void*)phys;

}

void pmm_free_page(void *page) {
    // LOCKS-TODO: later lock here

    pmm_node_t* node = VIRT(page);
    node->next = freelist_pmm_head;
    freelist_pmm_head = node;

    // LOCKS-TODO: later unlock here
}

void pmm_fill(u64 pages) {
    u64 pages_added = 0;

    while (freelist_pmm_fill_entry < memmap_request.response->entry_count && pages_added < pages) {
        struct limine_memmap_entry *entry = memmap_request.response->entries[freelist_pmm_fill_entry];

        if (entry->type != LIMINE_MEMMAP_USABLE) {
            freelist_pmm_fill_entry++;
            freelist_pmm_fill_offset = 0;
            continue;
        }

        u64 aligned_start = ALIGN_UP(entry->base, PAGE_SIZE);
        u64 aligned_end   = ALIGN_DOWN(entry->base + entry->length, PAGE_SIZE);

        // dont allocate memory in the first 1MB
        if (aligned_start < 0x100000ULL)
            aligned_start = 0x100000ULL;

        if (aligned_end <= aligned_start) {
            freelist_pmm_fill_entry++;
            freelist_pmm_fill_offset = 0;
            continue;
        }

        if (freelist_pmm_fill_offset == 0)
            freelist_pmm_fill_offset = aligned_start - entry->base;

        while (entry->base + freelist_pmm_fill_offset + PAGE_SIZE <= aligned_end && pages_added < pages) {
            u64 page_addr = entry->base + freelist_pmm_fill_offset;
            pmm_node_t *node = VIRT(page_addr);
            node->next = freelist_pmm_head;
            freelist_pmm_head = node;
            pages_added++;
            freelist_pmm_fill_offset += PAGE_SIZE;
        }

        if (entry->base + freelist_pmm_fill_offset + PAGE_SIZE > aligned_end) {
            freelist_pmm_fill_entry++;
            freelist_pmm_fill_offset = 0;
        }
    }

}

void pmm_init(void) {
    pmm_fill(512);
}
