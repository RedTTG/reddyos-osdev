#include "common.h"

uint64_t* kernel_pml4 = 0;
uint64_t kernel_cr3 = 0;

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

void vmm_init(void) {
    kernel_cr3 = get_cr3();
    kernel_pml4 = (uint64_t*)VIRT(kernel_cr3 & ~0xFFF);
}

uint64_t paging_current_cr3(void)
{
    return get_cr3();
}

uint64_t paging_kernel_cr3(void)
{
    return kernel_cr3;
}

static void invl_all(void)
{
    __asm__ volatile(
        "mov %%cr3, %%rax\n"
        "mov %%rax, %%cr3\n"
        ::: "rax", "memory"
    );
}

void paging_load_cr3(uint64_t cr3)
{
    set_cr3(cr3);

    // optional but important if reloading same CR3
    invl_all();
}

uint64_t* phys_to_virt(const uint64_t phys)
{
    // terminal_write("phys_to_virt: phys: ");
    // terminal_write_hex_u64(phys);
    // terminal_write("-> virt: ");
    // terminal_write_hex_u64(phys + hhdm_request.response->offset);
    // terminal_write("\n");
    return (uint64_t*)(phys + hhdm_request.response->offset);
}

uint64_t virt_to_phys(const uint64_t virt)
{
    return virt - hhdm_request.response->offset;
}

static void invlpg(void* addr)
{
    __asm__ volatile("invlpg (%0)" :: "r"(addr) : "memory");
}

static uint64_t* alloc_table(void)
{
    uint64_t phys = (uint64_t)pmm_alloc_page();
    uint64_t* table = phys_to_virt(phys);

    // terminal_write("Allocating new page table addr:");
    // terminal_write_hex_u64(phys);
    // terminal_write(" -> virt: ");
    // terminal_write_hex_u64((uint64_t)table);
    // terminal_write("\n");

    memset(table, 0, PAGE_SIZE);
    return table;
}

static uint64_t* get_next(uint64_t* table, const size_t index, const uint64_t flags)
{
    // terminal_write("get_next: table: ");
    // terminal_write_hex_u64((uint64_t)table);
    // terminal_write(", index: ");
    // terminal_write_u64(index);
    // terminal_write("\n");
    if (!(table[index] & PAGE_PRESENT))
    {
        uint64_t* new_table = alloc_table();

        table[index] =
            virt_to_phys((uint64_t)new_table) |
            PAGE_PRESENT |
            PAGE_WRITABLE |
            flags;
    }

    return phys_to_virt(table[index] & PAGE_MASK);
}

void vmm_map(const uint64_t* pml4, const uint64_t virt, const uint64_t phys, const uint64_t flags)
{
    // terminal_write("Map page: table addr: ");
    // terminal_write_hex_u64((uint64_t)pml4);
    // terminal_write(", virt: ");
    // terminal_write_hex_u64(virt);
    // terminal_write(", phys: ");
    // terminal_write_hex_u64(phys);
    // terminal_write(", flags: ");
    // if (flags & PAGE_WRITABLE) terminal_write("WRITABLE ");
    // if (flags & PAGE_USER) terminal_write("USER ");
    // if (flags & PAGE_WTHRU) terminal_write("WTHRU ");
    // if (flags & PAGE_NOCACHE) terminal_write("NOCACHE ");
    // if (flags & PAGE_NX) terminal_write("NX ");
    // terminal_write("\n");

    size_t pml4_i = PML4_INDEX(virt);
    size_t pdpt_i = PDPT_INDEX(virt);
    size_t pd_i   = PD_INDEX(virt);
    size_t pt_i   = PT_INDEX(virt);


    uint64_t* pdpt = get_next(pml4, pml4_i, flags);
    uint64_t* pd   = get_next(pdpt, pdpt_i, flags);
    uint64_t* pt   = get_next(pd, pd_i, flags);

    pt[pt_i] =
        (phys & PAGE_MASK) |
        PAGE_PRESENT |
        flags;

    invlpg((void*)virt);
}

void vmm_unmap(const uint64_t* pml4, const uint64_t virt)
{
    size_t pml4_i = PML4_INDEX(virt);
    size_t pdpt_i = PDPT_INDEX(virt);
    size_t pd_i   = PD_INDEX(virt);
    size_t pt_i   = PT_INDEX(virt);

    uint64_t* pdpt = phys_to_virt(pml4[pml4_i] & PAGE_MASK);
    uint64_t* pd   = phys_to_virt(pdpt[pdpt_i] & PAGE_MASK);
    uint64_t* pt   = phys_to_virt(pd[pd_i] & PAGE_MASK);

    pt[pt_i] = 0;

    invlpg((void*)virt);
}

uint64_t vmm_virt_to_phys(const uint64_t* pml4, const uint64_t virt)
{
    size_t pml4_i = PML4_INDEX(virt);
    size_t pdpt_i = PDPT_INDEX(virt);
    size_t pd_i   = PD_INDEX(virt);
    size_t pt_i   = PT_INDEX(virt);

    uint64_t* pdpt = phys_to_virt(pml4[pml4_i] & PAGE_MASK);
    uint64_t* pd   = phys_to_virt(pdpt[pdpt_i] & PAGE_MASK);
    uint64_t* pt   = phys_to_virt(pd[pd_i] & PAGE_MASK);

    return (pt[pt_i] & PAGE_MASK) | (virt & 0xFFF);
}

uint64_t vmm_map_page(const uint64_t phys, uint64_t flags)
{
    return phys; // placeholder for future per-process allocator
}