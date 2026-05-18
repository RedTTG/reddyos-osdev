#pragma once
#include "syscalls/fs/fs_flags.h"
#include "syscalls/fs/fs_errors.h"

int do_sys_open(const char* path, int flags, int mode);
ssize_t do_sys_read(int fd, void *buffer, size_t size);
ssize_t do_sys_write(int fd, const void* buffer, size_t size);
int do_sys_close(int fd);