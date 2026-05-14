#include <stdint.h>
#include <stdio.h>

#include "common.h"

static const char* exceptions[] =
{
    "Divide by zero",
    "Debug",
    "NMI",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault"
};

void isr_handler(interrupt_frame_t* frame)
{
    if (frame->interrupt_number == 14) {
        uint64_t cr2;
        __asm__ volatile ("mov %%cr2, %0" : "=r"(cr2));

        terminal_write("\nKERNEL PANIC: Page Fault\n");
        terminal_write("CR2: ");
        terminal_write_hex_u64(cr2);
        terminal_write("\n");

        terminal_write("ERR: ");
        terminal_write_hex_u64(frame->error_code);
        terminal_write("\n");

        terminal_write("RIP: ");
        terminal_write_hex_u64(frame->rip);
        terminal_write("\n");

        __asm__ volatile("cli");
        for (;;) {
            __asm__ volatile("hlt");
        }
    }

    char buffer[64];
    char* p = buffer;

    const char* msg = exceptions[frame->interrupt_number];
    while (*msg)
        *p++ = *msg++;

    *p++ = ' ';
    *p++ = '(';

    uint64_t n = frame->interrupt_number;

    char temp[21];
    int i = 0;

    do {
        temp[i++] = '0' + (n % 10);
        n /= 10;
    } while (n);

    while (i--)
        *p++ = temp[i];

    *p++ = ')';
    *p = 0;

    panic(buffer);
}