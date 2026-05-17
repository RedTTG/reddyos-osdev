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
        uint64_t cr2, cr3, gs;
        __asm__ volatile ("mov %%cr2, %0" : "=r"(cr2));
        __asm__ volatile ("mov %%cr3, %0" : "=r"(cr3));
        __asm__ volatile ("mov %%gs, %0" : "=r"(gs));

        terminal_write("\nKERNEL PANIC: Page Fault\n");

        terminal_write("CR3: ");
        terminal_write_hex_u64(cr3);
        terminal_write(" - ");
        if (cr3 == paging_kernel_cr3()) terminal_write("Kernel CR3");
        else {
            terminal_write("User CR3");
            thread_t* iter = thread_list;
            while (true) {
                if (iter->process && iter->process->address_space.cr3 == cr3) {
                    terminal_write(" (PID: ");
                    terminal_write_u64(iter->process->pid);
                    terminal_write(")");
                    break;
                }
                iter = iter->next;
                if (!iter) {
                    terminal_write(" (Unknown CR3)");
                    break;
                }
            }
        }
        terminal_write("\n");

        terminal_write("CR2: ");
        terminal_write_hex_u64(cr2);
        terminal_write("\n");
        terminal_write("GS: ");
        if (gs == 0) terminal_write("0 (User)");
        else {
            terminal_write_hex_u64(gs);
            terminal_write(" (Kernel)");
        }
        terminal_write("\n");

        terminal_write("ERR: ");
        terminal_write_hex_u64(frame->error_code);
        terminal_write("\n");
        terminal_write("PAGE INFO: ");
        if (frame->error_code & 0x1) terminal_write("Present | ");
        else terminal_write("Not Present!!! | ");
        if (frame->error_code & 0x2) terminal_write("Read/Write, ");
        else terminal_write("Read only, ");
        if (frame->error_code & 0x4) terminal_write("User, ");
        else terminal_write("Kernel, ");
        if (frame->error_code & 0x8) terminal_write("Reserved Write, ");
        else terminal_write("No Reserved Write, ");
        if (frame->error_code & 0x10) terminal_write("INSTRUCTION");
        else terminal_write("DATA");
        terminal_write("\n");

        terminal_write("RIP: ");
        terminal_write_hex_u64(frame->rip);
        terminal_write("\n");

        terminal_write("RSP: ");
        terminal_write_hex_u64(frame->rsp);
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