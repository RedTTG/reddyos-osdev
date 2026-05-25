#include "common.h"
#include <stdint.h>

#define IRQ_TABLE_ENTRY(n) (void*)irq_stub_##n,
void* irq_stub_table[16] = {
    IRQ_STUB_LIST(IRQ_TABLE_ENTRY)
};
#undef IRQ_TABLE_ENTRY

static irq_handler_t handlers[256] = {0};

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

static void pic_send_eoi(const uint8_t irq)
{
    if (irq >= 8)
        outb(PIC2_CMD, 0x20);

    outb(PIC1_CMD, 0x20);
}

void irq_register_handler(const uint8_t vector, const irq_handler_t handler) {
    handlers[vector] = handler;
}

void irq_dispatch(interrupt_frame_t* frame) {
    const uint8_t vector = frame->interrupt_number;

    if (handlers[vector]) {
        terminal_write("Handling IRQ: ");
        terminal_write_u64(vector);
        terminal_write("\n");
        handlers[vector](frame);
    }
    else {
        terminal_write("Unhandled IRQ: ");
        terminal_write_u8(vector);
        terminal_write("\n");
    }

}

void irq_handler(interrupt_frame_t* frame)
{
    // send EOI
    if (lapic_is_enabled()) {
        lapic_eoi();
    } else {
        pic_send_eoi(frame->interrupt_number - 32);
    }

    irq_dispatch(frame);
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