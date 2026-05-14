#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "common.h"
#include "framebuffer/basic_impl.h"

// Halt and catch fire function.
static void hcf(void)
{
    for (;;) {
        asm ("hlt");
    }
}

void timer_handler(const interrupt_frame_t* frame)
{
    // terminal_write("TIMER\n");
    schedule();
}

void task_a(void* arg)
{
    // ReSharper disable once CppDFAEndlessLoop
    while (1) {
        animate_square();
    }
}

void task_b(void* arg)
{
    file_t file;

    if (vfs_open("/bin/init", &file) == 0)
    {
        elf_info_t info = elf_load(&file);
        terminal_write("Elf entry address: ");
        terminal_write_hex_u64(info.entry);
        terminal_write(" base address: ");
        terminal_write_hex_u64(info.base);
        terminal_write("\n");

        // LOAD THE PROCESS :)
    } else {
        panic("Failed to open /bin/init");
    }
}

void task_c(void* arg) // USER-SPACE FUNC
{
    int x = 5;
    int y = x * 10;
    int z = y + 2;
}

void kmain(void) {
    // Ensure the bootloader actually understands our base revision (see spec).
    if (LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision) == false) {
        hcf();
    }

    terminal_init();
    terminal_write("Booted\n");


    // Ensure we got a framebuffer.
    if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1) {
        hcf();
    }

    // Fetch the first framebuffer.
    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];
    cube_init(framebuffer);

    // Interrupts and IRQs remap
    idt_init();
    irq_init();
    irq_install();

    // ACPI
    pmm_init();
    acpi_init();
    lapic_init();
    ioapic_init();
    __asm__ volatile("sti");
    // Interrupts
    ioapic_redirect_irq(0, 32); // PIT
    ioapic_redirect_irq(1, 33); // Keyboard

    irq_register_handler(32, timer_handler);

    // TarFS
    tarsf_limine_init();

    // Initialize the scheduler
    scheduler_init();

    thread_t* a = thread_create(task_a);
    thread_t* b = thread_create(task_b);

    scheduler_add(a);
    scheduler_add(b);


    // Finally start the timer
    lapic_timer_start();

    // We're done, just hang...
    hcf();
}