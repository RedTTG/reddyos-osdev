#pragma once

u64 do_sys_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);

static inline u64 sys_mmap(const syscall_args_t* args) {
    return do_sys_mmap((void*)args->arg1, (size_t)args->arg2, (int)args->arg3, (int)args->arg4, (int)args->arg5, (off_t)args->arg6);
}