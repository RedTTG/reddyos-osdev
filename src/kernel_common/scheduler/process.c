#include "common.h"

static uint64_t next_pid = 1;

process_t* process_create(const char* filename)
{
    file_t file = {0};
    elf_info_t info = {0};
    process_t* p;
    uint64_t old_cr3 = 0;

    if (!filename)
        return 0;

    if (vfs_open(filename, &file) != 0)
        return 0;

    if (elf_read_info(&file, &info) != 0)
        return 0;

    p = kmalloc(sizeof(process_t));

    if (!p)
        return 0;

    memset(p, 0, sizeof(*p));

    if (!process_fd_init(p)) {
        kfree(p);
        return 0;
    }

    p->address_space = paging_create_address_space();
    if (!p->address_space.cr3)
    {
        kfree(p);
        return 0;
    }

    p->entry_point = info.entry;
    p->user_stack_top = USER_STACK_TOP - 16;
    p->user_stack_bottom = USER_STACK_TOP - PAGE_SIZE;
    p->main_thread = 0;
    p->pid = next_pid++;

    void* stack_phys = pmm_alloc_page();

    if (!stack_phys)
    {
        paging_destroy_address_space(&p->address_space);
        kfree(p);
        return 0;
    }

    memset(VIRT(stack_phys), 0, PAGE_SIZE);

    // Map the stack
    vmm_map(
        &p->address_space,
        p->user_stack_bottom,
        (uint64_t)stack_phys,
        PAGE_USER | PAGE_WRITABLE | PAGE_PRESENT
    );

    if (elf_load_into_address_space(&p->address_space, &file, &info) != 0)
    {
        paging_destroy_address_space(&p->address_space);
        kfree(p);
        pmm_free_page(stack_phys);
        return 0;
    }

    return p;
}