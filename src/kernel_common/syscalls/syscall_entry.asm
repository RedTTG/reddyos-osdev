global syscall_entry
extern syscall_handler, current_thread

section .text

syscall_entry:
    ; At this point:
    ; - RCX contains the user return address (for sysretq)
    ; - R11 contains user RFLAGS
    ; - GS base is still pointing to user space (swapgs not done yet)
    ;
    ; Syscall arguments in registers (x86-64 syscall ABI):
    ; - RAX: syscall number
    ; - RDI, RSI, RDX, R10, R8, R9: arguments 1-6

    ; Swap GS base from user to kernel
    swapgs

    ; Save user RSP to percpu_data[0]
    mov [gs:0], rsp

    ; Get a kernel stack from current_thread
    ; Use R12 to avoid clobbering RAX (which contains the syscall number)
    lea r12, [rel current_thread]
    mov r12, [r12]       ; r12 = current_thread pointer

    ; If r12 is null, use emergency stack
    test r12, r12
    jz .use_emergency_stack

    ; Get kernel stack pointer from thread->kernel_stack (offset 8 in thread struct)
    ; thread struct layout: rsp(0), kernel_stack(8), user_stack(16), tid(24), ...
    mov rsp, [r12 + 8]
    add rsp, 0x1000      ; Point to top of kernel stack (kernel_stack + PAGE_SIZE)
    jmp .stack_ready

.use_emergency_stack:
    ; Use a fixed safe kernel stack address
    mov rsp, 0xffffffff80000000
    sub rsp, 0x1000

.stack_ready:
    ; Now we have a valid kernel RSP
    ; Build syscall_args_t struct on the stack
    ; struct layout: rax(0), rdi(8), rsi(16), rdx(24), r10(32), r8(40), r9(48) = 56 bytes

    ; Allocate space for syscall_args_t (56 bytes)
    sub rsp, 56

    ; Save syscall arguments to the struct
    ; RAX is still intact with the syscall number
    mov [rsp + 0],  rax   ; rax
    mov [rsp + 8],  rdi   ; rdi
    mov [rsp + 16], rsi   ; rsi
    mov [rsp + 24], rdx   ; rdx
    mov [rsp + 32], r10   ; r10
    mov [rsp + 40], r8    ; r8
    mov [rsp + 48], r9    ; r9

    ; Pass pointer to syscall_args_t as first argument (RDI)
    mov rdi, rsp

    ; Save user RIP and RFLAGS for return
    push rcx                   ; Save user RIP for sysretq
    push r11                   ; Save user RFLAGS for sysretq
    push r12                   ; Save R12 (callee-saved, we used it)

    ; Call the C handler (syscall_handler(syscall_args_t* args))
    call syscall_handler

    ; Restore registers
    pop r12                    ; Restore R12
    pop r11                    ; Restore user RFLAGS
    pop rcx                    ; Restore user RIP

    ; Clean up the syscall_args_t from the stack
    add rsp, 56

    ; Restore user RSP
    mov rsp, [gs:0]

    ; Swap GS back to user
    swapgs

    ; Return to user space with sysretq
    ; RCX = user RIP, R11 = user RFLAGS, RSP = user RSP, DS/ES = user DS
    sysretq

section .note.GNU-stack noalloc noexec nowrite progbits