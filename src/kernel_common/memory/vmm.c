#include "common.h"

address_space_t kernel_address_space;

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
    kernel_address_space.cr3 = get_cr3();
    kernel_address_space.pml4 = (uint64_t*)VIRT(kernel_address_space.cr3 & ~0xFFF);
}

void paging_copy_kernel_half(address_space_t* as)
{
    // Copy the shared kernel half (upper 256 PML4 entries) from the master
    // kernel page table so that all kernel text/data/interrupt handlers remain
    // mapped identically in every process address space.
    for (int i = 256; i < 512; i++)
    {
        as->pml4[i] = kernel_address_space.pml4[i];
    }
}

uint64_t paging_current_cr3(void)
{
    return get_cr3();
}

uint64_t paging_kernel_cr3(void)
{
    return kernel_address_space.cr3;
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

        /* Do not propagate NX or other leaf-only flags into intermediate
         * page-table entries. Only copy user/cache/wthru hints. */
        uint64_t table_flags = flags & (PAGE_USER | PAGE_WTHRU | PAGE_NOCACHE);

        table[index] =
            virt_to_phys((uint64_t)new_table) |
            PAGE_PRESENT |
            PAGE_WRITABLE |
            table_flags;
    }

    return phys_to_virt(table[index] & PAGE_MASK);
}

void vmm_map(const address_space_t* address_space, const uint64_t virt, const uint64_t phys, const uint64_t flags)
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


    uint64_t* pdpt = get_next(address_space->pml4, pml4_i, flags);
    uint64_t* pd   = get_next(pdpt, pdpt_i, flags);
    uint64_t* pt   = get_next(pd, pd_i, flags);

    pt[pt_i] =
        (phys & PAGE_MASK) |
        PAGE_PRESENT |
        flags;

    invlpg((void*)virt);
}

void vmm_unmap(const address_space_t* address_space, const uint64_t virt)
{
    size_t pml4_i = PML4_INDEX(virt);
    size_t pdpt_i = PDPT_INDEX(virt);
    size_t pd_i   = PD_INDEX(virt);
    size_t pt_i   = PT_INDEX(virt);

    uint64_t* pdpt = phys_to_virt(address_space->pml4[pml4_i] & PAGE_MASK);
    uint64_t* pd   = phys_to_virt(pdpt[pdpt_i] & PAGE_MASK);
    uint64_t* pt   = phys_to_virt(pd[pd_i] & PAGE_MASK);

    pt[pt_i] = 0;

    invlpg((void*)virt);
}

uint64_t vmm_virt_to_phys(const address_space_t* address_space, const uint64_t virt)
{
    size_t pml4_i = PML4_INDEX(virt);
    size_t pdpt_i = PDPT_INDEX(virt);
    size_t pd_i   = PD_INDEX(virt);
    size_t pt_i   = PT_INDEX(virt);

    uint64_t* pdpt = phys_to_virt(address_space->pml4[pml4_i] & PAGE_MASK);
    uint64_t* pd   = phys_to_virt(pdpt[pdpt_i] & PAGE_MASK);
    uint64_t* pt   = phys_to_virt(pd[pd_i] & PAGE_MASK);

    return (pt[pt_i] & PAGE_MASK) | (virt & 0xFFF);
}

address_space_t paging_create_address_space(void)
{
    address_space_t as;

    // 1. allocate new PML4
    as.pml4 = phys_to_virt((uint64_t)pmm_alloc_page());
    memset(as.pml4, 0, 4096);
    // 2. copy the shared kernel half (upper 256 entries)
    paging_copy_kernel_half(&as);

    // 3. convert to physical CR3
    as.cr3 = virt_to_phys((uint64_t)as.pml4);

    return as;
}

void paging_destroy_address_space(address_space_t* as)
{
    uint64_t* pml4 = as->pml4;

    // only user space (0–255 PML4 entries)
    for (int i = 0; i < 256; i++)
    {
        if (!(pml4[i] & PAGE_PRESENT))
            continue;

        uint64_t* pdpt = phys_to_virt(pml4[i] & PAGE_MASK);

        for (int j = 0; j < 512; j++)
        {
            if (!(pdpt[j] & PAGE_PRESENT))
                continue;

            uint64_t* pd = phys_to_virt(pdpt[j] & PAGE_MASK);

            for (int k = 0; k < 512; k++)
            {
                if (!(pd[k] & PAGE_PRESENT))
                    continue;

                uint64_t* pt = phys_to_virt(pd[k] & PAGE_MASK);

                for (int l = 0; l < 512; l++)
                {
                    if (!(pt[l] & PAGE_PRESENT))
                        continue;

                    uint64_t phys = pt[l] & PAGE_MASK;

                    pmm_free_page((void*)phys);
                }

                pmm_free_page((void*)virt_to_phys((uint64_t)pt));
            }

            pmm_free_page((void*)virt_to_phys((uint64_t)pd));
        }

        pmm_free_page((void*)virt_to_phys((uint64_t)pdpt));
    }

    // free PML4 itself
    pmm_free_page((void*)virt_to_phys((uint64_t)pml4));
}

uint64_t vmm_virt_to_phys_as(address_space_t* as, uint64_t virt)
{
    uint64_t* pml4 = as->pml4;

    size_t pml4_i = PML4_INDEX(virt);
    size_t pdpt_i = PDPT_INDEX(virt);
    size_t pd_i   = PD_INDEX(virt);
    size_t pt_i   = PT_INDEX(virt);

    uint64_t* pdpt = phys_to_virt(pml4[pml4_i] & PAGE_MASK);
    if (!(pml4[pml4_i] & PAGE_PRESENT)) return 0;

    uint64_t* pd = phys_to_virt(pdpt[pdpt_i] & PAGE_MASK);
    if (!(pdpt[pdpt_i] & PAGE_PRESENT)) return 0;

    uint64_t* pt = phys_to_virt(pd[pd_i] & PAGE_MASK);
    if (!(pd[pd_i] & PAGE_PRESENT)) return 0;

    if (!(pt[pt_i] & PAGE_PRESENT)) return 0;

    return (pt[pt_i] & PAGE_MASK) | (virt & 0xFFF);
}

void* vmm_translate(address_space_t* as, uint64_t virt)
{
    uint64_t phys = vmm_virt_to_phys_as(as, virt);

    if (!phys) {
        return 0;
    }
    // terminal_write("Vmm translate virt: ");
    // terminal_write_hex_u64(virt);
    // terminal_write(" -> phys: ");
    // terminal_write_hex_u64(phys);
    // terminal_write(" -> virt(kernel): ");
    // terminal_write_hex_u64((uint64_t)phys_to_virt(phys));
    // terminal_write("\n");

    return phys_to_virt(phys);
}

uint64_t vmm_map_page(const uint64_t phys, uint64_t flags)
{
    (void)flags;
    return phys; // placeholder for future per-process allocator
}