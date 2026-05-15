#include "common.h"

// Per-CPU data for syscall handler (stores user RSP)
__attribute__((aligned(16))) static percpu_data_t percpu_data = {0};

// Forward declaration for the current_thread
extern thread_t* current_thread;

void syscall_init(void)
{
    // -----------------------------------
    // Setup GS base for swapgs
    // -----------------------------------

    // IA32_KERNEL_GSBASE will be swapped in on syscall entry via swapgs
    wrmsr(IA32_KERNEL_GSBASE, (uint64_t)&percpu_data);



    // -----------------------------------
    // Enable SYSCALL/SYSRET
    // -----------------------------------

    wrmsr(
        IA32_EFER,
        rdmsr(IA32_EFER) | EFER_SCE
    );

    // -----------------------------------
    // STAR - Setup CS selectors
    // -----------------------------------

    // STAR format:
    // Bits 32-47: Kernel CS (and kernel DS is kernel CS + 8)
    // Bits 48-63: User code CS (and user DS is user CS + 8)

    uint64_t star =
        ((uint64_t)0x08 << 32) |   // kernel CS at 0x08
        ((uint64_t)0x1B << 48);    // user CS at 0x1B (ring-3 selector)

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
    // This clears the IF (Interrupt Flag) bit, disabling interrupts
    wrmsr(
        IA32_FMASK,
        (1 << 9) // IF
    );
}

long syscall_handler(syscall_args_t* args)
{
    // args->rax contains the syscall number
    // args->rdi, args->rsi, args->rdx, args->r10, args->r8, args->r9 contain the arguments

    terminal_write("syscall number: ");
    terminal_write_u64(args->rax);
    terminal_write("\n");

    switch (args->rax)
    {
        case 1: // Example syscall
            args->rax = args->rdi + 2;
            terminal_write("sys_test called with arg: ");
            terminal_write_u64(args->rdi);
            terminal_write(", returning: ");
            terminal_write_u64(args->rax);
            terminal_write("\n");
            return args->rax;
    }

    args->rax = (long)-1;
    return -1;
}