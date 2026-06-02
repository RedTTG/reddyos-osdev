#include <stdint.h>
#include <stdio.h>

#include "common.h"

static isr_handler_t handlers[32] = {0};

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

static void debug_interrupt_frame(const interrupt_frame_t* frame)
{
    // Debug all registers that will popped later
    // r15, r14, r13, r12;
    // r11, r10, r9, r8;
    // rsi, rdi, rbp, rdx;
    // rcx, rbx, rax;
    terminal_write("Registers debug: \nisr: ");
    terminal_write_u64(frame->interrupt_number);
    terminal_write(" rip: ");
    terminal_write_hex_u64(frame->rip);
    terminal_write(" cs: ");
    terminal_write_hex_u64(frame->cs);
    terminal_write(" rflags: ");
    terminal_write_hex_u64(frame->rflags);
    terminal_write(" rsp: ");
    terminal_write_hex_u64(frame->rsp);
    terminal_write(" ss: ");
    terminal_write_hex_u64(frame->ss);
    terminal_write("\nR15: ");
    terminal_write_hex_u64(frame->r15);
    terminal_write(" R14: ");
    terminal_write_hex_u64(frame->r14);
    terminal_write(" R13: ");
    terminal_write_hex_u64(frame->r13);
    terminal_write(" R12: ");
    terminal_write_hex_u64(frame->r12);
    terminal_write("\nR11: ");
    terminal_write_hex_u64(frame->r11);
    terminal_write(" R10: ");
    terminal_write_hex_u64(frame->r10);
    terminal_write(" R9: ");
    terminal_write_hex_u64(frame->r9);
    terminal_write(" R8: ");
    terminal_write_hex_u64(frame->r8);
    terminal_write("\nRSI: ");
    terminal_write_hex_u64(frame->rsi);
    terminal_write(" RDI: ");
    terminal_write_hex_u64(frame->rdi);
    terminal_write(" RBP: ");
    terminal_write_hex_u64(frame->rbp);
    terminal_write(" RDX: ");
    terminal_write_hex_u64(frame->rdx);
    terminal_write("\nRCX: ");
    terminal_write_hex_u64(frame->rcx);
    terminal_write(" RBX: ");
    terminal_write_hex_u64(frame->rbx);
    terminal_write(" RAX: ");
    terminal_write_hex_u64(frame->rax);
    terminal_write("\n");
}

void isr_handler(interrupt_frame_t* frame)
{
    //debug_interrupt_frame(frame);
    if (handlers[frame->interrupt_number]) {
        if (handlers[frame->interrupt_number](frame))
            return;
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

void isr_register_handler(const uint8_t vector, const isr_handler_t handler) {
    handlers[vector] = handler;
}