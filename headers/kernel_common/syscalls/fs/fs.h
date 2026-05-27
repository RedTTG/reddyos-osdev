// ReSharper disable CppParameterMayBeConst
#pragma once
#include <sys/types.h>
#include "syscalls/syscalls.h"

u64 do_sys_open(const char* path, int flags, int mode);
u64 do_sys_read(uint fd, void *buffer, size_t size);
u64 do_sys_write(uint fd, const void* buffer, size_t size);
u64 do_sys_close(uint fd);
u64 do_sys_ioctl(uint fd, uint64_t cmd, uint64_t arg);
u64 do_sys_stat(const char* filename, struct stat* buffer);
u64 do_sys_fstat(uint fd, struct stat* buffer);
off_t do_sys_lseek(uint fd, off_t offset, int whence);

static inline u64 sys_open(const syscall_args_t* args) {
    return do_sys_open((const char*)args->arg1, (int)args->arg2, (int)args->arg3);
}

static inline u64 sys_close(const syscall_args_t* args) {
    return do_sys_close((uint)args->arg1);
}
static inline u64 sys_read(const syscall_args_t* args) {
    return do_sys_read((uint)args->arg1, (void*)args->arg2, (size_t)args->arg3);
}

static inline u64 sys_write(const syscall_args_t* args) {
    return do_sys_write((uint)args->arg1, (const void*)args->arg2, (size_t)args->arg3);
}

static inline u64 sys_ioctl(const syscall_args_t* args) {
    return do_sys_ioctl((uint)args->arg1, (uint64_t)args->arg2, (uint64_t)args->arg3);
}

static inline u64 sys_lseek(const syscall_args_t* args) {
    return do_sys_lseek((uint)args->arg1, (off_t)args->arg2, (int)args->arg3);
}
static inline u64 sys_stat(const syscall_args_t* args) {
    return do_sys_stat((const char*)args->arg1, (struct stat*)args->arg2);
}

static inline u64 sys_fstat(const syscall_args_t* args) {
    return do_sys_fstat((uint)args->arg1, (struct stat*)args->arg2);
}
