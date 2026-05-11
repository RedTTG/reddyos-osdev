#include "common.h"
#include <stdint.h>

void pic_remap(int offset1, int offset2)
{
    uint8_t a1 = inb(PIC1_DATA);
    uint8_t a2 = inb(PIC2_DATA);

    outb(PIC1_CMD, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC2_CMD, ICW1_INIT | ICW1_ICW4);
    io_wait();

    outb(PIC1_DATA, offset1);
    io_wait();
    outb(PIC2_DATA, offset2);
    io_wait();

    outb(PIC1_DATA, 4);
    io_wait();
    outb(PIC2_DATA, 2);
    io_wait();

    outb(PIC1_DATA, ICW4_8086);
    outb(PIC2_DATA, ICW4_8086);

    outb(PIC1_DATA, a1);
    outb(PIC2_DATA, a2);
}

void irq_init(void)
{
    pic_remap(32, 40);

    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}

void irq_install(void)
{
    for (int i = 0; i < 16; i++)
    {
        idt_set_gate(32 + i, irq_stub_table[i], 0x8E);
    }
}

static void pic_send_eoi(uint8_t irq)
{
    if (irq >= 8)
        outb(PIC2_CMD, 0x20);

    outb(PIC1_CMD, 0x20);
}

void irq_handler(interrupt_frame_t* frame)
{
    if (frame->interrupt_number == 33) {
        uint8_t scancode = inb(0x60);
        char buf[4];

        terminal_write("Keyboard interrupt received: 0x");
        const char hex[] = "0123456789abcdef";
        buf[0] = hex[(scancode >> 4) & 0xF];
        buf[1] = hex[scancode & 0xF];
        buf[2] = '\n';
        buf[3] = 0;
        terminal_write(buf);
    }

    //char buf[32];
    //itoa(frame->interrupt_number, buf, 10);
    //terminal_write(buf);
    //terminal_write("\n");

    // send EOI
    if (lapic_is_enabled()) {
        lapic_eoi();
    } else {
        pic_send_eoi(frame->interrupt_number - 32);
    }
}

void pic_unmask_irq(uint8_t irq)
{
    uint16_t port;
    uint8_t value;

    if (irq < 8)
        port = PIC1_DATA;
    else
    {
        port = PIC2_DATA;
        irq -= 8;
    }

    value = inb(port) & ~(1 << irq);
    outb(port, value);
}

void pic_disable(void)
{
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}