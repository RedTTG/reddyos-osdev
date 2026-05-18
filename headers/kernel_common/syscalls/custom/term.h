#pragma once
#include "syscalls/syscalls.h"

u64 do_sys_term(char c);

static u64 sys_term(const syscall_args_t* args) {
    return do_sys_term((char)args->arg1);
}