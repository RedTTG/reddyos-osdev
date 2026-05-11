#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "common.h"

// Halt and catch fire function.
static void hcf(void) {
    for (;;) {
        asm ("hlt");
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

    // Print a nice pattern to screen as an example.
    // Note: we assume the framebuffer model is RGB with 32-bit pixels.
    volatile uint32_t *fb_ptr = framebuffer->address;
    for (size_t y = 0; y < framebuffer->height; y++) {
        for (size_t x = 0; x < framebuffer->width; x++) {
            uint32_t nX = x * 255 / framebuffer->width;
            uint32_t nY = y * 255 / framebuffer->height;
            fb_ptr[y * (framebuffer->pitch / 4) + x] = (nY << 8) | nX;
        }
    }

    // Interrupts and IRQs remap
    idt_init();
    irq_init();
    irq_install();

    // Basic clock
    pit_init(100);

    // Start interrupts
    // if (apic_is_enabled() || false) {
    //     pic_disable();
    //     apic_init();
    // } else {
    //     pic_unmask_irq(0);
    //     pic_unmask_irq(1);
    // }



    // ACPI
    pmm_init();
    acpi_init();
    lapic_map();
    ioapic_map();
    lapic_init();
    ioapic_init();
    __asm__ volatile("sti");

    ioapic_redirect_irq(0, 32); // PIT
    ioapic_redirect_irq(1, 33); // Keyboard

    // We're done, just hang...
    hcf();
}