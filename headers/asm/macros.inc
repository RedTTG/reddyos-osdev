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
    test byte [rsp + 24], 3   ; CS is at iret/syscall frame
    jz %%done

    swapgs
    ;push rax

    ;mov rax, [gs:0]
    ;cmp byte [rax + 0x18], 0
    ;je %%skip

    ;mov byte [rax + 0x18], 0 ; in_user = 0
    ;swapgs

%%skip:
    pop rax
%%done:
%endmacro
%endif