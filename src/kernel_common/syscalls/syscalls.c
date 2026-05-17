#include "common.h"

// Per-CPU data for syscall handler (stores user RSP)
__attribute__((aligned(16))) static percpu_data_t percpu_data = {0};

// Forward declaration for the current_thread
extern thread_t* current_thread;

void syscall_init(void)
{
    // Setup GS base
    wrmsr(IA32_KERNEL_GSBASE, (uint64_t)&percpu_data);
    wrmsr(IA32_GSBASE, 0);



    // Enable syscalls
    wrmsr(
        IA32_EFER,
        rdmsr(IA32_EFER) | EFER_SCE
    );

    // Star selectors
    uint64_t star =
        ((uint64_t)0x08 << 32) |   // kernel CS at 0x08
        ((uint64_t)0x1B << 48);    // user CS at 0x1B (ring-3 selector)

    wrmsr(IA32_STAR, star);

    // Set syscall entry LSTAR
    wrmsr(
        IA32_LSTAR,
        (uint64_t)syscall_entry
    );

    // Disable interrupts on entering syscall entry
    wrmsr(
        IA32_FMASK,
        (1 << 9) // IF
    );
}

long syscall_handler(syscall_args_t* args)
{
    // args->rax contains the syscall number
    // args->rdi, args->rsi, args->rdx, args->r10, args->r8, args->r9 contain the arguments

    // terminal_write("syscall number: ");
    // terminal_write_u64(args->rax);
    // terminal_write("\n");
    // terminal_write("Current thread ID: ");
    // terminal_write_u64(current_thread ? current_thread->tid : 0);
    // terminal_write("\n");
    // if (current_thread && current_thread->process) {
    //     terminal_write("Current PID: ");
    //     terminal_write_u64(current_thread->process->pid);
    //     terminal_write("\n");
    // }

    switch (args->rax)
    {
        case 1: // Example syscall
            args->rax = args->rdi + 2;
            return args->rax;
    }

    args->rax = (long)-1;
    return -1;
}