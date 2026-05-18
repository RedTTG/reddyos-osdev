#pragma once
#include "syscalls/fs/fs_flags.h"
int sys_open(const char* path, int flags, int mode);
long sys_read(int fd, char *buffer, size_t size);