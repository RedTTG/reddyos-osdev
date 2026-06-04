global syscall_entry
extern syscall_handler

section .text

syscall_entry:
    swapgs
    ;swapgs_if_necessary;

    ; SysV C code expects DF=0.
    cld

    ; Save user RSP and load kernel RSP
    mov [gs:8], rsp
    mov rsp, [gs:16]
    push qword [gs:8]


    ; syscall_frame_t
    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rdi
    push rsi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    ; Pass pointer to syscall_frame_t as first argument (RDI).
    mov rdi, rsp
    call syscall_handler

    ; Restore user registers (pop all, then overwrite rcx/r11 below).
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rsi
    pop rdi
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax

    ; Restore user RSP
    pop qword [gs:8]
    mov rsp, [gs:8]

    swapgs
    o64 sysret

section .note.GNU-stack noalloc noexec nowrite progbits

