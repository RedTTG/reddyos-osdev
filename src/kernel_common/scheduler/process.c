#include "common.h"

static uint64_t next_pid = 1;

process_t* process_create(const char* filename)
{
    // TODO: Update process_create
    return 0;
    // file_t file = {0};
    // elf_info_t info = {0};
    // process_t* p;
    // uint64_t old_cr3 = 0;
    // uint64_t stack_phys = 0;
    //
    // if (!filename)
    //     return 0;
    //
    // if (vfs_open(filename, &file) != 0)
    //     return 0;
    //
    // if (elf_read_info(&file, &info) != 0)
    //     return 0;
    //
    // p = pmm_alloc_virt_page();
    //
    // if (!p)
    //     return 0;
    //
    // memset(p, 0, sizeof(*p));
    //
    // p->cr3 = paging_create_address_space();
    // if (!p->cr3)
    // {
    //     pmm_free_virt_page(p);
    //     return 0;
    // }
    //
    // p->entry_point = info.entry;
    // p->user_stack_top = USER_STACK_TOP;
    // p->user_stack_bottom = USER_STACK_TOP - PAGE_SIZE;
    // p->main_thread = 0;
    // p->pid = next_pid++;
    //
    // stack_phys = (uint64_t)pmm_alloc_page();
    // if (!stack_phys)
    // {
    //     pmm_free_page((void*)p->cr3);
    //     pmm_free_virt_page(p);
    //     return 0;
    // }
    //
    // memset(VIRT(stack_phys), 0, PAGE_SIZE);
    //
    // // Map the stack
    // old_cr3 = paging_current_cr3();
    // paging_load_cr3(p->cr3);
    // map_page(
    //     p->user_stack_bottom,
    //     stack_phys,
    //     PAGE_USER | PAGE_WRITABLE | PAGE_PRESENT
    // );
    // paging_load_cr3(old_cr3);
    //
    // if (elf_load_into_cr3(p->cr3, &file, &info) != 0)
    // {
    //     pmm_free_page((void*)stack_phys);
    //     pmm_free_page((void*)p->cr3);
    //     pmm_free_virt_page(p);
    //     return 0;
    // }
    //
    // return p;
}