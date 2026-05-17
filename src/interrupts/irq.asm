global irq_stub_table
extern irq_handler
%include "macros.inc"

%assign i 0
%rep 16
extern irq_stub_%+i
%assign i i+1
%endrep

%macro IRQ 1
irq_stub_%1:
    push 0
    push %1+32
    jmp irq_common
%endmacro

section .text

IRQ 0
IRQ 1
IRQ 2
IRQ 3
IRQ 4
IRQ 5
IRQ 6
IRQ 7
IRQ 8
IRQ 9
IRQ 10
IRQ 11
IRQ 12
IRQ 13
IRQ 14
IRQ 15

irq_common:
    swapgs_if_necessary;
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

    mov rdi, rsp
    call irq_handler

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

    add rsp, 16
    swapgs
    iretq

section .note.GNU-stack noalloc noexec nowrite progbits
