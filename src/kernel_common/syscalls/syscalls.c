#include "common.h"
#include "errors.h"
#include "syscalls/def.h"

// Per-CPU data for syscall handler (stores user RSP)
__attribute__((aligned(16))) percpu_data_t percpu_data = {0};

#define X(id, fn) [id] = fn,
syscall_fun_t syscall_table[] = {
    SYSCALLS
};
#undef X
const u64 syscallCount = sizeof(syscall_table) / sizeof(syscall_table[0]);

void syscall_init(void)
{
    // Setup GS base
    // IA32_GSBASE is active in kernel mode (after being swapped by thread_entry_user)
    // IA32_KERNEL_GSBASE is the "user" slot (what user code will use after swapgs in thread_entry_user)
    wrmsr(IA32_GSBASE, (uint64_t)&percpu_data);
    wrmsr(IA32_KERNEL_GSBASE, 0);

    // Enable syscalls
    wrmsr(
        IA32_EFER,
        rdmsr(IA32_EFER) | EFER_SCE | EFER_NXE
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

    // Disable interrupts and the direction flag on entering syscall entry.
    wrmsr(
        IA32_FMASK,
        (1 << 9) | (1 << 10) // IF | DF
    );
}

u64 syscall_handler(syscall_frame_t* frame)
{
    // args->rax contains the syscall number
    // args->rdi, args->rsi, args->rdx, args->r10, args->r8, args->r9 contain the arguments

    // terminal_write("syscall number: ");
    // terminal_write_u64(frame->rax);
    // terminal_write("\n");
    // terminal_write("Current thread ID: ");
    // terminal_write_u64(current_thread ? current_thread->tid : 0);
    // terminal_write("\n");
    // if (current_thread && current_thread->process) {
    //     terminal_write("Current PID: ");
    //     terminal_write_u64(current_thread->process->pid);
    //     terminal_write("\n");
    // }

    if (frame->rax >= syscallCount) {
        goto enosys;
    }
    syscall_fun_t fun = syscall_table[frame->rax];
    if (fun == NULL) {
        enosys:
        terminal_write("\n!====\nUnknown syscall: ");
        terminal_write_u64(frame->rax);
        terminal_write("\n");
        terminal_write("Current thread ID: ");
        terminal_write_u64(current_thread ? current_thread->tid : 0);
        terminal_write("\n");
        if (current_thread && current_thread->process) {
            terminal_write("Current PID: ");
            terminal_write_u64(current_thread->process->pid);
            terminal_write("\n!====\n\n");
        }

        frame->rax = -ENOSYS;
        return frame->rax;
    }
    syscall_args_t syscall_args[] = {
        frame->rax,
        frame->rdi,
        frame->rsi,
        frame->rdx,
        frame->r10,
        frame->r8,
        frame->r9,
    };

    frame->rax = fun(syscall_args);
    return frame->rax;
}