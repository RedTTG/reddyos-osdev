global arch_switch_thread, thread_entry_kernel, thread_entry_user
extern dump_context
section .text

; arch_switch_thread(thread_t* old, thread_t* next)
arch_switch_thread:
    fxsave [rdi + 0x40]

    push rbx
    push rbp
    push r12
    push r13
    push r14
    push r15
    mov [rdi], rsp
    mov rsp, [rsi]
    fxrstor [rsi + 0x40]
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    pop rbx
    pop rax
    jmp rax
    ; Should never reach here. Fault if it does so we get a clear trap instead
    ud2

thread_entry_kernel:
    pop rdi
    sti
    ret
thread_entry_user:
    pop rcx ; future user rip
    pop rdx ; future user rsp
    pop r11 ; future user rflags

    or  r11, 0x200 ; enable interrupts in user mode

    mov rsp, rdx

    swapgs
    o64 sysret
