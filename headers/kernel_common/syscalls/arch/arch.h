#pragma once

u64 do_sys_arch_prctl(u64 cmd, const void* pointer);

static inline u64 sys_arch_prctl(const syscall_args_t* args) {
    return do_sys_arch_prctl((u64)args->arg1, (void*)args->arg2);
}