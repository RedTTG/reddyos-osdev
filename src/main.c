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

void load_init(void* arg)
{
    static const char* init_filename = "/bin/init";
    (void)arg;
    process_t* process = process_create(init_filename);
    thread_t* user_thread;

    if (!process)
        panic("Failed to create init process");

    user_thread = user_thread_create(process);
    if (!user_thread)
        panic("Failed to create user thread for init process");

    // terminal_write("Created user thread for ");
    // terminal_write(init_filename);
    // terminal_write("\n");
    scheduler_add(user_thread);
}

void init_interrupts(void) {
    // Ensure GDT and TSS are installed so the CPU will switch to a safe
    // kernel stack (rsp0) when interrupts occur from user space.
    gdt_install();

    // Interrupts and IRQs remap
    idt_init();
    irq_init();
    irq_install();

    __asm__ volatile("sti");
    acpi_init();
    terminal_write("ACPI initialized successfully\n");
    lapic_init();
    ioapic_init();

    // Interrupts
    ioapic_redirect_irq(0, 32); // PIT
    ioapic_redirect_irq(1, 33); // Keyboard
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

void register_devices(void) {
    fb_device_init();
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
    init_interrupts();
    // fpu_init();

    irq_register_handler(32, timer_handler);

    // Filesystem
    init_filesystem();
    register_devices();

    // Syscalls
    syscall_init();

    // Initialize the scheduler
    scheduler_init();

    thread_t* a = thread_create(load_init);

    scheduler_add(a);

    // Finally start the timer
    lapic_timer_start();

    // We're done, just hang...
    hcf();
}