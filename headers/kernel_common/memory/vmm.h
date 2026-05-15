#pragma once

#define PAGE_PRESENT  (1ULL << 0)
#define PAGE_WRITABLE    (1ULL << 1)
#define PAGE_USER     (1ULL << 2)
#define PAGE_WTHRU (1ULL << 3)
#define PAGE_NOCACHE   (1ULL << 4)
#define PAGE_NX       (1ULL << 63)
#define PAGE_NOEXEC       (1ULL << 63)

// #define PAGE_MASK 0xFFFFFFFFFFFFF000ULL
#define PAGE_MASK 0x000FFFFFFFFFF000ULL

#define USER_CODE_BASE   0x400000
#define USER_STACK_TOP   0x70000000
#define USER_HEAP_BASE   0x80000000

#define PML4_INDEX(x) (((x) >> 39) & 0x1FF)
#define PDPT_INDEX(x) (((x) >> 30) & 0x1FF)
#define PD_INDEX(x)   (((x) >> 21) & 0x1FF)
#define PT_INDEX(x)   (((x) >> 12) & 0x1FF)
#define memvirt phys_to_virt
#define memphys virt_to_phys
#define VIRT(p) ((void*)((uint64_t)(p) + hhdm_request.response->offset))

typedef struct address_space
{
    uint64_t* pml4;   // HHDM pointer
    uint64_t cr3;     // physical address for CPU
} address_space_t;

extern address_space_t kernel_address_space;

void vmm_init(void);

void paging_load_cr3(uint64_t cr3);
uint64_t paging_kernel_cr3(void);
uint64_t paging_current_cr3(void);

uint64_t* phys_to_virt(uint64_t phys);
uint64_t virt_to_phys(uint64_t virt);

void vmm_map(const address_space_t* address_space, uint64_t virt, uint64_t phys, uint64_t flags);
void vmm_unmap(const address_space_t* address_space, const uint64_t virt);
uint64_t vmm_virt_to_phys(const address_space_t* address_space, const uint64_t virt);

inline void map_page(uint64_t virt, uint64_t phys, uint64_t flags) {
    vmm_map(&kernel_address_space, virt, phys, flags);
}
void unmap_page(uint64_t virt);

address_space_t paging_create_address_space(void);
void paging_destroy_address_space(address_space_t* as);

void* vmm_translate(address_space_t* as, uint64_t virt);

uint64_t vmm_map_page(uint64_t phys, uint64_t flags);