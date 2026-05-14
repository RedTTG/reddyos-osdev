#include "common.h"
typedef struct core_dump {
    uint64_t r15, r14, r13, r12, rbp, rbx;
    uint64_t rsp;
    uint64_t rip;
    uint64_t cr2;
    uint64_t err;
} core_dump_t;

__attribute__((noinline))
void dump_context(void)
{
    core_dump_t dump = {};

    asm volatile (
        // save callee-saved registers
        "mov %%r15, %0\n"
        "mov %%r14, %1\n"
        "mov %%r13, %2\n"
        "mov %%r12, %3\n"
        "mov %%rbp, %4\n"
        "mov %%rbx, %5\n"

        // save rsp
        "mov %%rsp, %6\n"

        // get return address (RIP)
        "lea (%%rip), %%rax\n"
        "mov %%rax, %7\n"
        : "=m"(dump.r15),
          "=m"(dump.r14),
          "=m"(dump.r13),
          "=m"(dump.r12),
          "=m"(dump.rbp),
          "=m"(dump.rbx),
          "=m"(dump.rsp),
          "=m"(dump.rip)
        :
        : "rax", "memory"
    );

    terminal_write("Core dump:\n");
    terminal_write("RIP: ");
    terminal_write_hex_u64(dump.rip);
    terminal_write("\nRSP: ");
    terminal_write_hex_u64(dump.rsp);
    terminal_write("\nRBP: ");
    terminal_write_hex_u64(dump.rbp);
    terminal_write("\nR15: ");
    terminal_write_hex_u64(dump.r15);
    terminal_write("\nR14: ");
    terminal_write_hex_u64(dump.r14);
    terminal_write("\nR13: ");
    terminal_write_hex_u64(dump.r13);
    terminal_write("\nR12: ");
    terminal_write_hex_u64(dump.r12);
    terminal_write("\nRBX: ");
    terminal_write_hex_u64(dump.rbx);
    terminal_write("\nCR2: ");
    terminal_write_hex_u64(dump.cr2);
    terminal_write("\nError code: ");
    terminal_write_hex_u64(dump.err);
    terminal_write("\n");

    while (1) {
        asm volatile ("cli; hlt");
    }
}