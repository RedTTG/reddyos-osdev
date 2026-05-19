#include "common.h"
void panic(const char* msg)
{
    terminal_write("\nKERNEL PANIC: ");
    terminal_write(msg);
    terminal_write("\n");

    __asm__ volatile("cli");
    while (1)
        __asm__ volatile("hlt");
}

void panic_isr(const char* msg, const interrupt_frame_t* frame)
{
    uint64_t cr2, cr3, gs;
    __asm__ volatile ("mov %%cr2, %0" : "=r"(cr2));
    __asm__ volatile ("mov %%cr3, %0" : "=r"(cr3));
    __asm__ volatile ("mov %%gs, %0" : "=r"(gs));

    terminal_write("\nKERNEL PANIC: ");
    terminal_write(msg);
    terminal_write("\n");

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
    /* Present / Not present */
    if (frame->error_code & 0x1)
        terminal_write("Protection Violation | ");
    else
        terminal_write("Page Not Present | ");

    /* Read / Write */
    if (frame->error_code & 0x2)
        terminal_write("Write | ");
    else
        terminal_write("Read | ");

    /* User / Kernel */
    if (frame->error_code & 0x4)
        terminal_write("User Mode | ");
    else
        terminal_write("Kernel Mode | ");

    /* Reserved bit violation */
    if (frame->error_code & 0x8)
        terminal_write("RSVD Violation | ");
    else
        terminal_write("No RSVD Issue | ");
    /* Instruction fetch vs data access */
    if (frame->error_code & 0x10)
        terminal_write("Instruction Fetch (NX violation)\n");
    else
        terminal_write("Data Access\n");

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