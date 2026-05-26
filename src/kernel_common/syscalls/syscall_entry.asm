global syscall_entry
extern syscall_handler, current_thread

section .text

syscall_entry:
    swapgs

    ; Save the user return state for iretq.
    ; percpu_data layout: user_rsp(0), user_rcx(8), user_r11(16)
    mov [gs:0], rsp           ; Save user RSP at offset 0
    mov [gs:8], rcx           ; Save user RCX at offset 8
    mov [gs:16], r11          ; Save user R11 at offset 16

    ; SysV C code expects DF=0.
    cld

    ; Get a kernel stack from current_thread without clobbering user registers.
    cmp qword [rel current_thread], 0
    je .use_emergency_stack

    mov rsp, [rel current_thread]
    mov rsp, [rsp + 8]
    add rsp, 0x1000           ; Point to top of kernel stack (kernel_stack + PAGE_SIZE)
    jmp .stack_ready

.use_emergency_stack:
    ; Use a fixed safe kernel stack address.
    mov rsp, 0xffffffff80000000
    sub rsp, 0x1000

.stack_ready:
    ; Allocate space for syscall_frame_t (15 * 8 bytes = 120 bytes).
    sub rsp, 120

    ; Save the full register frame expected by syscall_handler().
    mov [rsp + 0],  rax
    mov [rsp + 8],  rbx
    mov rax, [gs:8]
    mov [rsp + 16], rax       ; user RIP (stored in rcx slot)
    mov [rsp + 24], rdx
    mov [rsp + 32], rbp
    mov [rsp + 40], rdi
    mov [rsp + 48], rsi
    mov [rsp + 56], r8
    mov [rsp + 64], r9
    mov [rsp + 72], r10
    mov rax, [gs:16]
    mov [rsp + 80], rax       ; user RFLAGS (stored in r11 slot)
    mov [rsp + 88], r12
    mov [rsp + 96], r13
    mov [rsp + 104], r14
    mov [rsp + 112], r15

    ; Pass pointer to syscall_frame_t as first argument (RDI).
    mov rdi, rsp
    call syscall_handler

    ; Preserve the return value and build the iretq frame below the saved frame.
    mov [rsp + 0], rax
    sub rsp, 40               ; SS(8), RSP(8), RFLAGS(8), CS(8), RIP(8)

    mov rdx, [rsp + 56]       ; saved user RIP (frame.rcx)
    mov [rsp + 0], rdx        ; RIP
    mov qword [rsp + 8], 0x1b ; CS (user code segment, ring 3)
    mov rdx, [gs:16]          ; user RFLAGS from percpu_data
    mov [rsp + 16], rdx       ; RFLAGS
    mov rdx, [gs:0]           ; user RSP from percpu_data
    mov [rsp + 24], rdx       ; RSP (user stack pointer)
    mov qword [rsp + 32], 0x23; SS (user data segment, ring 3)

    ; Restore user registers from the saved frame.
    mov rbx, [rsp + 48]
    mov rdx, [rsp + 64]
    mov rbp, [rsp + 72]
    mov rdi, [rsp + 80]
    mov rsi, [rsp + 88]
    mov r8,  [rsp + 96]
    mov r9,  [rsp + 104]
    mov r10, [rsp + 112]
    mov r11, [rsp + 120]
    mov r12, [rsp + 128]
    mov r13, [rsp + 136]
    mov r14, [rsp + 144]
    mov r15, [rsp + 152]

    swapgs
    iretq
