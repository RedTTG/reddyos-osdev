#pragma once
#include <stdint.h>

#define PAGE_PRESENT   (1ULL << 0)
#define PAGE_WRITABLE  (1ULL << 1)
#define PAGE_USER      (1ULL << 2)
#define PAGE_WTHRU     (1ULL << 3)
#define PAGE_NOCACHE   (1ULL << 4)
#define PAGE_NOEXEC    (1ULL << 63)

#define PML4_INDEX(x) (((x) >> 39) & 0x1FF)
#define PDPT_INDEX(x) (((x) >> 30) & 0x1FF)
#define PD_INDEX(x)   (((x) >> 21) & 0x1FF)
#define PT_INDEX(x)   (((x) >> 12) & 0x1FF)

#define USER_CODE_BASE   0x400000
#define USER_STACK_TOP   0x70000000
#define USER_HEAP_BASE   0x80000000

#define VIRT(p) ((void*)((uint64_t)(p) + hhdm_request.response->offset))

void paging_init(void);
uint64_t paging_current_cr3(void);
uint64_t paging_kernel_cr3(void);
void paging_load_cr3(uint64_t cr3);
uint64_t paging_create_address_space(void);

void map_page(uint64_t virt, uint64_t phys, uint64_t flags);
void identity_map_region(uint64_t start, uint64_t size);

// Debug helper: walk the page tables of the given CR3 and return the physical
// page base for the supplied virtual address (or 0 if not mapped).
uint64_t paging_virt_to_phys(uint64_t cr3, uint64_t virt);

