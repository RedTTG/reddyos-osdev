#pragma once

u64 do_sys_arch_prctl(int op, u64 addr);

static u64 sys_arch_prctl(const syscall_args_t* args) {
    return do_sys_arch_prctl((int)args->arg1, args->arg2);
}