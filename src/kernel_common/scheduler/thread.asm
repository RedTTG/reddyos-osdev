global arch_switch_thread, thread_entry_kernel, thread_entry_user
extern dump_context
section .text

; arch_switch_thread(Thread* old, Thread* next)
arch_switch_thread:
    push rbx
    push rbp
    push r12
    push r13
    push r14
    push r15
    mov [rdi], rsp
    mov rsp, [rsi]
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    pop rbx
    ret

thread_entry_kernel:
    pop rdi
    sti
    ret
thread_entry_user:
    cli ; disable interrupts
    pop rcx ; future user rip
    pop rdx ; future user rsp

    mov r11, 0x002 ; set syscall/sysret compatibility mode
    mov rsp, rdx ; set user stack pointer

    sti ; enable interrupts
    o64 sysret ; return to user mode, rip = rcx, rsp = rdx, r11 = 0x202
