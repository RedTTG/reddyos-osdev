#include "common.h"
#include "syscalls/fs/fs_errors.h"

u64 do_sys_open(const char* path, int flags, int mode) {
    return process_fd_open(current_thread->process, path, flags, mode);
}

u64 do_sys_read(const int fd, void *buffer, const size_t size) {
    if (!buffer || size == 0)
        return -EINVAL;

    file_t* file = process_unpack_fd(current_thread->process, fd);
    if (!file)
        return -EBADF;

    return vfs_read(file, buffer, size);
}

u64 do_sys_write(int fd, const void *buffer, size_t size) {
    if (!buffer || size == 0)
        return -EINVAL;

    file_t* file = process_unpack_fd(current_thread->process, fd);
    if (!file)
        return -EBADF;

    return vfs_write(file, buffer, size);
}

u64 do_sys_close(int fd) {
    return process_fd_close(current_thread->process, fd);
}

