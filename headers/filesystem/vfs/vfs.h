#pragma once
#include "files.h"

int vfs_open(const char* path, file_t* out, int flags, int mode);

int vfs_read(
    file_t* file,
    void* buffer,
    uint64_t size
);

void vfs_close(file_t* file);