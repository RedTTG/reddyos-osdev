#include <uacpi/utilities.h>

#include "common.h"
#include "interrupts/gdt.h"

// Halt and catch fire function.
static __attribute__((noreturn)) void hcf(void)
{
    for (;;) {
        asm ("hlt");
    }
}

void timer_handler(const interrupt_frame_t* frame)
{
    (void)frame;
    schedule();
}

bool page_fault_handler(const interrupt_frame_t* frame)
{
    if (current_thread && CUR_PROCESS) {
        // Get the memory being accessed
        uint64_t addr;
        __asm__ volatile ("mov %%cr2, %0" : "=r"(addr));

        // Check for stack growth
        if (addr + PAGE_SIZE >= CUR_PROCESS->user_stack_bottom_max &&
            addr < CUR_PROCESS->user_stack_bottom && grow_user_stack(current_thread->process)) {
            return true;
        }

        // Check for VMA
        vm_area_t* vma = vma_find(CUR_PROCESS, addr);
        if (vma) {
            if (vma->type == VMA_ANON) {
                void* phys_page = pmm_alloc_page();
                if (!phys_page)
                    return false;
                // TODO: Adjust the actual flags
                vmm_map(&CUR_PROCESS->address_space,ALIGN_DOWN(addr, PAGE_SIZE), (u64)phys_page, PAGE_PRESENT | PAGE_USER | PAGE_WRITABLE);
                memset(VIRT(phys_page), 0, PAGE_SIZE);
                // terminal_write("PF ");
                // terminal_write_hex_u64(addr);
                // terminal_write(" VMA ");
                // terminal_write_hex_u64(vma->start);
                // terminal_write(" PG ");
                // terminal_write_hex_u64(ALIGN_DOWN(addr, PAGE_SIZE));
                // terminal_write("\n");
                return true;
            }
        }
    }

    // PAGE FAULT
    return false;
}

void load_init(void* arg)
{
    static const char* init_filename = "/bin/init";
    (void)arg;
    // terminal_write("Loading init process from ");
    // terminal_write(init_filename);
    // terminal_write("\n");

    process_t* process = process_create(init_filename);
    thread_t* user_thread;

    if (!process)
        panic("Failed to create init process");

    user_thread = user_thread_create(process);
    if (!user_thread)
        panic("Failed to create user thread for init process");

    scheduler_add(user_thread);
    // terminal_write("Created user thread for ");
    // terminal_write(init_filename);
    // terminal_write("\n");
}

void init_basic_interrupts(void) {
    // Ensure GDT and TSS are installed so the CPU will switch to a safe
    // kernel stack (rsp0) when interrupts occur from user space.
    gdt_install();

    // Interrupts and IRQs remap
    idt_init();
    irq_init();
    irq_install();

    __asm__ volatile("sti");
}

void init_memory(void) {
    // Paging and physical memory management
    pmm_init();
    vmm_init();
    // terminal_write("Memory initialized!\n");
}

void init_filesystem(void) {
    tarsf_limine_init();
    fd_init();

    // Mount devfs at /dev
    devfs_init();
    if (mount_fs("/dev", devfs_root()) != 0) {
        terminal_write("Warning: failed to mount /dev\n");
    }
}

void init_apic(void) {
    lapic_init();
    ioapic_init();
    ioapic_redirect_irq(0, 32); // PIT
    ioapic_redirect_irq(1, 33); // Keyboard
}

void register_devices(void) {
    fb_device_init();
    terminal_device_init(); // stdin/stdout/stderr
}

void kmain(void) {
    // Ensure the bootloader actually understands our base revision (see spec).
    if (LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision) == false) {
        hcf();
    }

    terminal_init();
    terminal_write("Booted\n");
    // terminal_write("CR3 addr at boot: ");
    // uint64_t cr3 = (uint64_t)memvirt(paging_current_cr3());
    // terminal_write_hex_u64(cr3);
    // terminal_write("\n");

    // Initialize
    init_memory();
    init_basic_interrupts();
    acpi_init();
    clocks_init();
    // acpi_namespace_init();
    init_apic();
    fpu_init();

    irq_register_handler(32, timer_handler);
    isr_register_handler(14, page_fault_handler);

    // Filesystem
    init_filesystem();
    register_devices();

    // Syscalls
    syscall_init();

    // Initialize the scheduler
    scheduler_init();

    #ifdef KMALLOC_CANARY
    terminal_write("KMALLOC_CANARY self-test: ");
    if (kmalloc_canary_selftest()) {
        terminal_write("PASS\n");
    } else {
        terminal_write("FAIL\n");
        panic("KMALLOC_CANARY self-test failed");
    }
    #else
    terminal_write("KMALLOC_CANARY self-test: N/A\n");
    #endif

    thread_t* a = thread_create(load_init);

    scheduler_add(a);

    // Finally start the timer
    lapic_timer_start();

    terminal_write("Kernel bring-up complete!\n");

    // We're done, just hang...
    hcf();
}