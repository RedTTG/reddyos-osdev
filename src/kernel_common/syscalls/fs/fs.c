#include "common.h"
#include "syscalls/fs/fs.h"

#include <asm-generic/errno-base.h>

#include "../../../../headers/kernel_common/syscalls/errors.h"

u64 do_sys_open(const char* path, int flags, int mode) {
    return process_fd_open(current_thread->process, path, flags, mode);
}

u64 do_sys_read(const uint fd, void *buffer, const size_t size) {
    if (!buffer || size == 0)
        return -EINVAL;

    file_t* file = process_unpack_fd(current_thread->process, fd);
    if (!file)
        return -EBADF;

    return vfs_read(file, buffer, size);
}

u64 do_sys_write(uint fd, const void *buffer, size_t size) {
    if (!buffer || size == 0)
        return -EINVAL;

    file_t* file = process_unpack_fd(current_thread->process, fd);
    if (!file)
        return -EBADF;

    return vfs_write(file, buffer, size);
}

u64 do_sys_close(uint fd) {
    return process_fd_close(current_thread->process, fd);
}

u64 do_sys_ioctl(uint fd, uint64_t cmd, uint64_t arg) {
    file_t* file = process_unpack_fd(current_thread->process, fd);
    if (!file)
        return -EBADF;

    return vfs_ioctl(file, cmd, arg);
}

uint64_t do_sys_stat(const char *filename, stat_t *buffer) {
    vnode_t* node = vfs_find_vnode(filename);
    if (!node)
        return -ENOENT;

    return vfs_stat(node, buffer);
}

u64 do_sys_fstat(uint fd, stat_t* buffer) {
    file_t* file = process_unpack_fd(current_thread->process, fd);
    if (!file)
        return -EBADF;

    return vfs_stat(file->vnode, buffer);
}


off_t do_sys_lseek(uint fd, off_t offset, int whence) {
    file_t* file = process_unpack_fd(current_thread->process, fd);
    if (!file)
        return -EBADF;

    return vfs_lseek(file, offset, whence);
}