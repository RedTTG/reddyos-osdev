%ifndef MACROS_INC
%define MACROS_INC

%macro swapgs_if_user 0
    test byte [rsp + 24], 3   ; CS is at iret/syscall frame
    jz %%done

    swapgs
    ;push rax
    ;mov rax, [gs:0]
    ;mov byte [rax + 0x18], 1   ; in_user = 1
    ;pop rax

%%done:
%endmacro

%macro swapgs_if_exit 0
    ; After popping the 15 saved GPRs in isr_common/irq_common, the hardware
    ; iret frame begins at [rsp]. The saved CS is therefore at [rsp + 8].
    test byte [rsp + 8], 3
    jz %%done

    swapgs
%%done:
%endmacro
%endif