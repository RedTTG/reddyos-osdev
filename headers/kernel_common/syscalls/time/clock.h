#pragma once

u64 do_sys_clock_gettime(clockid_t clock, struct timespec* ts);

static inline u64 sys_clock_gettime(const syscall_args_t* args) {
    return do_sys_clock_gettime((clockid_t)args->arg1, (struct timespec*)args->arg2);
}