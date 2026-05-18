#pragma once
#include "files.h"
#include "stat.h"

// lseek whence constants
#define SEEK_SET 0  // Beginning of file
#define SEEK_CUR 1  // Current position
#define SEEK_END 2  // End of file

void* vfs_find_vnode(const char* path);
int vfs_open(const char* path, file_t* out, int flags, int mode);

ssize_t vfs_read(file_t *file, void *buffer, uint64_t size);

ssize_t vfs_write(file_t *file, const void *buffer, uint64_t size);

int vfs_ioctl(file_t *file, uint64_t cmd, void *arg);

int vfs_stat(vnode_t *node, stat_t *buffer);

off_t vfs_lseek(file_t *file, off_t offset, int whence);

void vfs_close(file_t* file);

// Directory helpers
int vfs_is_dir(const char* path);
int vfs_dir_count(const char* path);
