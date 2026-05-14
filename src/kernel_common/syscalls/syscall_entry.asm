global syscall_entry
extern syscall_handler

section .text

syscall_entry:
    swapgs

    ; save user rsp
    mov [gs:0], rsp

    ; switch to kernel stack
    mov rsp, [gs:8]

    push rcx
    push r11

    call syscall_handler

    pop r11
    pop rcx

    swapgs
    sysretq;

section .note.GNU-stack noalloc noexec nowrite progbits