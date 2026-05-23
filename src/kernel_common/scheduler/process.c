#include "common.h"
#include "process_stack.h"
#include "fs_flags.h"

static uint64_t next_pid = 1;


void setup_process_stack(process_t* p, const char* filename) {
    paging_load_cr3(p->address_space.cr3);
    u64* stack_top = (void*)p->rsp;
    u64* sp = stack_top;

    /*
     * Put strings onto stack first.
     */
    char *prog_name_ptr =
        push_string(
            &sp,
            filename
        );

    /*
     * Align before pushing pointers.
     */
    sp = (uint64_t*)
        ((uintptr_t)sp & ~0xFULL);

    /*
     * Reserve AT_RANDOM storage so libc has a valid pointer during startup.
     */
    push_u64(&sp, 0);
    push_u64(&sp, 0);
    uint8_t* random_ptr = (uint8_t*)sp;

    /*
     * Auxiliary vector
     */

    push_auxv(
        &sp,
        AT_PAGESZ,
        PAGE_SIZE
    );

    push_auxv(
        &sp,
        AT_ENTRY,
        p->entry_point
    );

    push_auxv(
        &sp,
        AT_RANDOM,
        (uint64_t)random_ptr
    );

    push_auxv(
        &sp,
        AT_EXECFN,
        (uint64_t)prog_name_ptr
    );

    push_auxv(&sp, AT_NULL, 0);

    /*
     * envp[]
     */

    push_ptr(&sp, NULL);

    /*
     * argv[]
     */

    push_ptr(&sp, NULL);
    push_ptr(&sp, prog_name_ptr);

    /*
     * argc
     */

    push_u64(&sp, 1);


    p->rsp = (uint64_t)sp;
    terminal_write("Set up process stack, rsp: ");
    terminal_write_hex_u64(p->rsp);
    terminal_write("\n");
    paging_load_cr3(paging_kernel_cr3());
}

process_t* process_create(const char* filename)
{
    file_t file = {0};
    elf_info_t info = {0};
    process_t* p;

    if (!filename)
        return 0;

    if (vfs_open(filename, &file, O_RDONLY, 0) != 0)
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
    terminal_write("Created address space for process, CR3: ");
    terminal_write_hex_u64(p->address_space.cr3);
    terminal_write("\n");

    p->entry_point = info.entry;
    p->rsp = USER_STACK_TOP;
    p->user_stack_bottom = USER_STACK_TOP - (USER_STACK_PAGES * PAGE_SIZE);
    p->user_stack_bottom_max = p->user_stack_bottom;
    p->main_thread = 0;
    p->pid = next_pid++;
    terminal_write("Stack top: ");
    terminal_write_hex_u64(p->rsp);
    terminal_write(" bottom: ");
    terminal_write_hex_u64(p->user_stack_bottom);
    terminal_write("\n");

    void* stack_phys_pages[USER_STACK_PAGES] = {0};

    for (u64 i = 0; i < USER_STACK_PAGES; i++)
    {
        void* stack_phys = pmm_alloc_page();

        if (!stack_phys)
        {
            for (size_t j = 0; j < i; j++)
                pmm_free_page(stack_phys_pages[j]);

            paging_destroy_address_space(&p->address_space);
            kfree(p);
            return 0;
        }

        stack_phys_pages[i] = stack_phys;
        memset(VIRT(stack_phys), 0, PAGE_SIZE);

        // Map the stack
        terminal_write("Map virt (");
        terminal_write_u64(i);
        terminal_write("): ");
        terminal_write_hex_u64(p->user_stack_bottom + (i * PAGE_SIZE));
        terminal_write("\n");
        vmm_map(
            &p->address_space,
            p->user_stack_bottom + (i * PAGE_SIZE),
            (uint64_t)stack_phys,
            PAGE_USER | PAGE_WRITABLE | PAGE_PRESENT
        );
    }

    terminal_write("addr space: ");
    terminal_write_hex_u64((uint64_t)&p->address_space);
    terminal_write("\n");
    if (elf_load_into_address_space(&p->address_space, &file, &info) != 0)
    {
        paging_destroy_address_space(&p->address_space);
        kfree(p);
        for (size_t i = 0; i < USER_STACK_PAGES; i++)
            pmm_free_page(stack_phys_pages[i]);
        return 0;
    }

    // Set up user stack
    setup_process_stack(p, filename);

    return p;
}