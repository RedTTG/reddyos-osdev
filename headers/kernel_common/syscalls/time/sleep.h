#pragma once

u64 do_sys_nanosleep(struct timespec* duration, struct timespec* remaining);

static inline u64 sys_nanosleep(const syscall_args_t* args) {
    return do_sys_nanosleep((struct timespec*)args->arg1, (struct timespec*)args->arg2);
}