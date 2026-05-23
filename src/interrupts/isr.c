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
    if (frame->interrupt_number == 14 && current_thread->process) {
        uint64_t cr2;
        __asm__ volatile ("mov %%cr2, %0" : "=r"(cr2));
        // TODO: ALLOCATE MORE PAGES TO STACK
        terminal_write("Process stack bottom: ");
        terminal_write_hex_u64(current_thread->process->user_stack_bottom);
        terminal_write("\n");
    }
    // if (frame->interrupt_number == 13 && current_thread && current_thread->process) {
    //     // Only handle faults that originated from user mode (CPL == 3)
    //     if ((frame->cs & 0x3) == 3) {
    //         void* instr = vmm_kernel_ap(&current_thread->process->address_space, frame->rip);
    //         if (instr) {
    //             uint8_t opcode = *(uint8_t*)instr;
    //             // HLT opcode is 0xF4
    //             if (opcode == 0xF4) {
    //                 frame->rip += 1;
    //                 return;
    //             }
    //         }
    //     }
    // }

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

    panic_isr(buffer, frame);
}