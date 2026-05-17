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
    pop rcx ; future user rip
    pop rdx ; future user rsp
    pop r11 ; future user rflags

    or  r11, 0x200 ; enable interrupts in user mode

    ; Build the interrupt frame for iretq
    ; iretq pops: RIP, CS, RFLAGS, RSP, SS (in that order, from top of frame)
    sub rsp, 40               ; allocate space for the frame

    mov qword [rsp + 32], 0x23      ; SS (user data segment, ring 3)
    mov qword [rsp + 24], rdx       ; RSP (user stack pointer)
    mov qword [rsp + 16], r11       ; RFLAGS (user flags)
    mov qword [rsp + 8],  0x1b      ; CS (user code segment, ring 3)
    mov qword [rsp + 0],  rcx       ; RIP (user return address)

    swapgs
    iretq
