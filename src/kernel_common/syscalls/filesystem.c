#include "common.h"

long sys_open(const char* path) {
    return process_fd_open(current_thread->process, path);
}

long sys_read(const int fd, char *buffer, const size_t size) {
    if (buffer == NULL || size == 0)
        return -1;
    file_t* file = process_unpack_fd(current_thread->process, fd);
    if (file == NULL)
        return -1;

    const int read = vfs_read(file, buffer, size);

    if (read <= 0)
        return -1;
    return read;
}

