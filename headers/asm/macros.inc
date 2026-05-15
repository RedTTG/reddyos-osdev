%ifndef MACROS_INC
%define MACROS_INC

%macro swapgs_if_necessary 0
    cmp qword [rsp + 8], 0x08
    je %%done

    swapgs

%%done:
%endmacro

%endif