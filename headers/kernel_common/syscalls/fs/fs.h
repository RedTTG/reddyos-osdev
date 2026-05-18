// ReSharper disable CppParameterMayBeConst
#pragma once
#include "syscalls/syscalls.h"

u64 do_sys_open(const char* path, int flags, int mode);
u64 do_sys_read(int fd, void *buffer, size_t size);
u64 do_sys_write(int fd, const void* buffer, size_t size);
u64 do_sys_close(int fd);

static u64 sys_open(const syscall_args_t* args) {
    return do_sys_open((const char*)args->arg1, (int)args->arg2, (int)args->arg3);
}

static u64 sys_close(const syscall_args_t* args) {
    return do_sys_close((int)args->arg1);
}
static u64 sys_read(const syscall_args_t* args) {
    return do_sys_read((int)args->arg1, (void*)args->arg2, (size_t)args->arg3);
}

static u64 sys_write(const syscall_args_t* args) {
    return do_sys_write((int)args->arg1, (const void*)args->arg2, (size_t)args->arg3);
}