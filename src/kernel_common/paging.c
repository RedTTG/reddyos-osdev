#include "common.h"

static uint64_t get_cr3(void)
{
    uint64_t val;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(val));
    return val & ~0xFFF;
}

void map_page(uint64_t virt, uint64_t phys, uint64_t flags)
{
    virt &= ~0xFFFULL;
    phys &= ~0xFFFULL;

    uint64_t* pml4 = (uint64_t*)VIRT(get_cr3() & ~0xFFF);

    uint64_t pml4_i = PML4_INDEX(virt);
    uint64_t pdpt_i = PDPT_INDEX(virt);
    uint64_t pd_i   = PD_INDEX(virt);
    uint64_t pt_i   = PT_INDEX(virt);

    if ((uint64_t)pml4 & 0xFFF)
        panic("CR3 not page aligned");

    // PML4
    if (!(pml4[pml4_i] & PAGE_PRESENT))
    {
        uint64_t new_table = (uint64_t)pmm_alloc_page();
        if (!new_table)
            panic("Out of physical memory while allocating PML4 table");
        pml4[pml4_i] = new_table | PAGE_PRESENT | PAGE_WRITABLE;

        memset(VIRT(new_table), 0, 0x1000);
    }

    uint64_t* pdpt = (uint64_t*)VIRT(pml4[pml4_i] & ~0xFFF);

    // PDPT
    if (!(pdpt[pdpt_i] & PAGE_PRESENT))
    {
        uint64_t new_table = (uint64_t)pmm_alloc_page();
        if (!new_table)
            panic("Out of physical memory while allocating PDPT table");
        pdpt[pdpt_i] = new_table | PAGE_PRESENT | PAGE_WRITABLE;

        memset(VIRT(new_table), 0, 0x1000);
    }

    uint64_t* pd = (uint64_t*)VIRT(pdpt[pdpt_i] & ~0xFFF);

    // PD
    if (!(pd[pd_i] & PAGE_PRESENT))
    {
        uint64_t new_table = (uint64_t)pmm_alloc_page();
        if (!new_table)
            panic("Out of physical memory while allocating PD table");
        pd[pd_i] = new_table | PAGE_PRESENT | PAGE_WRITABLE;

        memset(VIRT(new_table), 0, 0x1000);
    }

    uint64_t* pt = (uint64_t*)VIRT(pd[pd_i] & ~0xFFF);

    // PT entry
    pt[pt_i] = phys | flags;

    __asm__ volatile ("invlpg (%0)" :: "r" ((void*)virt) : "memory");
}

void identity_map_region(uint64_t start, uint64_t size)
{
    for (uint64_t i = 0; i < size; i += 0x1000)
    {
        map_page(start + i, start + i,
            PAGE_PRESENT | PAGE_WRITABLE | PAGE_NOCACHE);
    }
}