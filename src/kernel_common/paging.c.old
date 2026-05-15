#include "common.h"

static uint64_t kernel_cr3 = 0;

static uint64_t get_cr3(void)
{
    uint64_t val;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(val));
    return val & ~0xFFF;
}

static void set_cr3(uint64_t cr3)
{
    __asm__ volatile ("mov %0, %%cr3" :: "r"(cr3) : "memory");
}

void paging_init(void)
{
    kernel_cr3 = get_cr3();
}

uint64_t paging_current_cr3(void)
{
    return get_cr3();
}

uint64_t paging_kernel_cr3(void)
{
    return kernel_cr3;
}

void paging_load_cr3(uint64_t cr3)
{
    set_cr3(cr3);
}

uint64_t paging_create_address_space(void)
{
    if (!kernel_cr3)
        paging_init();

    uint64_t new_phys = (uint64_t)pmm_alloc_page();
    if (!new_phys)
        return 0;

    uint64_t* new_pml4 = (uint64_t*)VIRT(new_phys);
    uint64_t* kernel_pml4 = (uint64_t*)VIRT(kernel_cr3);

    memset(new_pml4, 0, PAGE_SIZE);

    for (size_t i = 256; i < 512; i++)
        new_pml4[i] = kernel_pml4[i];

    return new_phys;
}

void map_page(uint64_t virt, uint64_t phys, uint64_t flags)
{
    terminal_write("Mapping page: virt=");
    terminal_write_hex_u64(virt);
    terminal_write(" phys=");
    terminal_write_hex_u64(phys);
    terminal_write(" flags=");
    if (flags & PAGE_PRESENT) terminal_write("PRESENT ");
    if (flags & PAGE_WRITABLE) terminal_write("WRITABLE ");
    if (flags & PAGE_USER) terminal_write("USER ");
    terminal_write("\n");
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
        pml4[pml4_i] = new_table | PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;

        memset(VIRT(new_table), 0, 0x1000);
    }

    uint64_t* pdpt = (uint64_t*)VIRT(pml4[pml4_i] & ~0xFFF);

    // PDPT
    if (!(pdpt[pdpt_i] & PAGE_PRESENT))
    {
        uint64_t new_table = (uint64_t)pmm_alloc_page();
        if (!new_table)
            panic("Out of physical memory while allocating PDPT table");
        pdpt[pdpt_i] = new_table | PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;

        memset(VIRT(new_table), 0, 0x1000);
    }

    uint64_t* pd = (uint64_t*)VIRT(pdpt[pdpt_i] & ~0xFFF);

    // PD
    if (!(pd[pd_i] & PAGE_PRESENT))
    {
        uint64_t new_table = (uint64_t)pmm_alloc_page();
        if (!new_table)
            panic("Out of physical memory while allocating PD table");
        pd[pd_i] = new_table | PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;

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