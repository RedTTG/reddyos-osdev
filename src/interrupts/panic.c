#include "common.h"

__attribute__((__noreturn__))
void panic(const char* msg)
{
    terminal_write("\nKERNEL PANIC: ");
    terminal_write(msg);
    terminal_write("\n");

    __asm__ volatile("cli");
    while (1)
        __asm__ volatile("hlt");
}

static void print_gpf_error(u64 error_code)
{
    terminal_write("GPF INFO: ");

    if (error_code == 0) {
        terminal_write("No selector information\n");
        return;
    }

    terminal_write("External: ");
    terminal_write((error_code & 0x1) ? "Yes | " : "No | ");

    terminal_write("IDT: ");
    terminal_write((error_code & 0x2) ? "Yes | " : "No | ");

    terminal_write("TI/LDT: ");
    terminal_write((error_code & 0x4) ? "Yes | " : "No | ");

    terminal_write("Index: ");
    terminal_write_hex_u64(error_code >> 3);
    terminal_write("\n");
}

static void print_pf_tables(address_space_t* as, u64 cr2, u64 error_code)
{
    terminal_write("PAGE INFO: ");
    /* Present / Not present */
    if (error_code & 0x1)
        terminal_write("Protection Violation | ");
    else
        terminal_write("Page Not Present | ");

    /* Read / Write */
    if (error_code & 0x2)
        terminal_write("Write | ");
    else
        terminal_write("Read | ");

    /* User / Kernel */
    if (error_code & 0x4)
        terminal_write("User Mode | ");
    else
        terminal_write("Kernel Mode | ");

    /* Reserved bit violation */
    if (error_code & 0x8)
        terminal_write("RSVD Violation | ");
    else
        terminal_write("No RSVD Issue | ");
    /* Instruction fetch vs data access */
    if (error_code & 0x10)
        terminal_write("Instruction Fetch (NX violation)\n");
    else
        terminal_write("Data Access\n");
    tables_debug(as->pml4, cr2);
}

__attribute__((__noreturn__))
void panic_isr(const char* msg, const interrupt_frame_t* frame)
{
    uint64_t cr2, cr3, gs_selector, gs_base;
    __asm__ volatile ("mov %%cr2, %0" : "=r"(cr2));
    __asm__ volatile ("mov %%cr3, %0" : "=r"(cr3));
    __asm__ volatile ("mov %%gs, %0" : "=r"(gs_selector));
    gs_base = rdmsr(IA32_GSBASE);

    terminal_write("\nKERNEL PANIC: ");
    terminal_write(msg);
    terminal_write("\n");

    terminal_write("CR3: ");
    terminal_write_hex_u64(cr3);
    terminal_write(" - ");
    address_space_t* as = &kernel_address_space;
    if (cr3 == paging_kernel_cr3()) terminal_write("Kernel CR3");
    else {
        terminal_write("User CR3");
        thread_t* iter = thread_list;

        while (true) {
            if (!iter) {
                terminal_write(" (Unknown CR3)");
                break;
            }
            if (iter->process && iter->process->address_space.cr3 == cr3) {
                as = &iter->process->address_space;
                terminal_write(" (PID: ");
                terminal_write_u64(iter->process->pid);
                terminal_write(")");
                break;
            }
            iter = iter->next;
        }
    }
    terminal_write("\n");
    if (frame->interrupt_number == 14) {
        terminal_write("CR2: ");
        terminal_write_hex_u64(cr2);
        terminal_write("\n");
    }
    terminal_write("GS selector: ");
    terminal_write_hex_u64(gs_selector);
    terminal_write("\n");

    terminal_write("GS base: ");
    terminal_write_hex_u64(gs_base);
    if (gs_base == (uint64_t)&percpu_data) {
        terminal_write(" (Kernel)");
    } else if (gs_base == 0) {
        terminal_write(" (User)");
    } else {
        terminal_write(" (Unknown)");
    }
    terminal_write("\n");

    terminal_write("ERR: ");
    terminal_write_hex_u64(frame->error_code);
    terminal_write("\n");
    if (frame->interrupt_number == 13) {
        print_gpf_error(frame->error_code);
    }
    if (frame->interrupt_number == 14) {
        print_pf_tables(as, cr2, frame->error_code);
    }

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