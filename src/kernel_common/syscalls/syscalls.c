#include "common.h"

void syscall_init(void)
{
    // -----------------------------------
    // Enable SYSCALL/SYSRET
    // -----------------------------------

    wrmsr(
        IA32_EFER,
        rdmsr(IA32_EFER) | EFER_SCE
    );

    // -----------------------------------
    // STAR
    // -----------------------------------

    uint64_t star =
        ((uint64_t)0x08 << 32) |   // kernel CS
        ((uint64_t)0x1B << 48);    // user CS

    wrmsr(IA32_STAR, star);

    // -----------------------------------
    // syscall RIP
    // -----------------------------------

    wrmsr(
        IA32_LSTAR,
        (uint64_t)syscall_entry
    );

    // -----------------------------------
    // clear flags on syscall entry
    // -----------------------------------

    wrmsr(
        IA32_FMASK,
        (1 << 9) // IF
    );
}
void syscall_handler(void) {
    // TODO: Make syscall handler
    terminal_write("syscall\n");
    for (;;)
        __asm__ volatile("hlt");
}