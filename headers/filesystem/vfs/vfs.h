#pragma once
#include "files.h"

int vfs_open(const char* path, file_t* out, int flags, int mode);

ssize_t vfs_read(file_t *file, void *buffer, uint64_t size);

ssize_t vfs_write(file_t *file, const void *buffer, uint64_t size);

int vfs_ioctl(file_t *file, uint64_t request, void *arg);

void vfs_close(file_t* file);

// Directory helpers
int vfs_is_dir(const char* path);
int vfs_dir_count(const char* path);
