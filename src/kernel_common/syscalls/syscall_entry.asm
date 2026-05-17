global syscall_entry
extern syscall_handler, current_thread

section .text

syscall_entry:
    swapgs

    ; Save user registers to percpu_data for later restoration
    ; percpu_data layout: user_rsp(0), user_rcx(8), user_r11(16)
    mov [gs:0], rsp           ; Save user RSP at offset 0
    mov [gs:8], rcx           ; Save user RCX at offset 8
    mov [gs:16], r11          ; Save user R11 at offset 16

    ; Get a kernel stack from current_thread
    ; Use R12 to avoid clobbering RAX (which contains the syscall number)
    lea r12, [rel current_thread]
    mov r12, [r12]            ; r12 = current_thread pointer

    ; If r12 is null, use emergency stack
    test r12, r12
    jz .use_emergency_stack

    ; Get kernel stack pointer from thread->kernel_stack (offset 8 in thread struct)
    ; thread struct layout: rsp(0), kernel_stack(8), user_stack(16), tid(24), ...
    mov rsp, [r12 + 8]
    add rsp, 0x1000           ; Point to top of kernel stack (kernel_stack + PAGE_SIZE)
    jmp .stack_ready

.use_emergency_stack:
    ; Use a fixed safe kernel stack address
    mov rsp, 0xffffffff80000000
    sub rsp, 0x1000

.stack_ready:
    ; Allocate space for syscall_args_t (56 bytes)
    sub rsp, 56

    ; Save syscall arguments to the struct on the stack
    ; RAX is still intact with the syscall number
    mov [rsp + 0],  rax       ; rax
    mov [rsp + 8],  rdi       ; rdi
    mov [rsp + 16], rsi       ; rsi
    mov [rsp + 24], rdx       ; rdx
    mov [rsp + 32], r10       ; r10
    mov [rsp + 40], r8        ; r8
    mov [rsp + 48], r9        ; r9

    ; Pass pointer to syscall_args_t as first argument (RDI)
    mov rdi, rsp

    call syscall_handler
    mov rax, [rsp + 0]

    add rsp, 56

    mov rbx, [gs:0]           ; user RSP
    mov rcx, [gs:8]           ; user RCX (return address)
    mov r11, [gs:16]          ; user R11 (RFLAGS)

    ; Build the interrupt frame for iretq (bottom to top on stack)
    ; iretq pops: RIP, CS, RFLAGS, RSP, SS (in that order, from top of frame)

    ; We need to prepare a kernel stack with the proper frame
    ; Current RSP is at the end of our working area, set it up as kernel temp
    sub rsp, 40               ; allocate space for: SS(8), RSP(8), RFLAGS(8), CS(8), RIP(8)

    ; Push in reverse order (build frame from bottom up)
    mov qword [rsp + 32], 0x23      ; SS (user data segment, ring 3)
    mov qword [rsp + 24], rbx       ; RSP (user stack pointer)
    mov qword [rsp + 16], r11       ; RFLAGS (user flags)
    mov qword [rsp + 8],  0x1b      ; CS (user code segment, ring 3)
    mov qword [rsp + 0],  rcx       ; RIP (user return address)

    swapgs
    iretq
