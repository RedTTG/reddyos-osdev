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
    (void)frame;

    static uint64_t ticks = 0;
    ticks++;
    if (ticks >= 100) {
        animate_square();
        ticks = 0;
    }
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

    ioapic_redirect_irq(0, 32); // PIT
    ioapic_redirect_irq(1, 33); // Keyboard

    irq_register_handler(32, timer_handler);

    lapic_timer_start();

    // We're done, just hang...
    hcf();
}