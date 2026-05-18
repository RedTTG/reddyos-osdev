#include "common.h"

file_t **global_file_table = NULL;
size_t global_file_capacity = 0;

void fd_init() {
    global_file_capacity = BASE_FILE_LIST_SIZE;

    global_file_table =
        kcalloc(global_file_capacity, sizeof(file_t*));
}

static bool of_extend_table() {
    const size_t new_capacity = global_file_capacity * 2;

    file_t **new_table = kcalloc(new_capacity, sizeof(file_t*));

    if (!new_table)
        return false;

    memcpy(
        new_table,
        global_file_table,
        global_file_capacity * sizeof(file_t*)
    );

    kfree(global_file_table);

    global_file_table = new_table;
    global_file_capacity = new_capacity;

    return true;
}

static bool process_fd_extend(process_t* process) {
    const size_t new_capacity = process->fd_capacity * 2;

    fd_t **new_table = kcalloc(new_capacity, sizeof(fd_t*));

    if (!new_table)
        return false;

    memcpy(
        new_table,
        process->fds,
        process->fd_capacity * sizeof(fd_t*)
    );

    kfree(process->fds);

    process->fds = new_table;
    process->fd_capacity = new_capacity;

    return true;
}

static int of_open(const char *path)
{
    file_t *file = kmalloc(sizeof(file_t));

    if (!file)
        return -1;

    if (vfs_open(path, file)<0) {
        kfree(file);
        return -1;
    }

    for (;;) {

        for (size_t i = 0; i < global_file_capacity; i++) {

            if (global_file_table[i] == NULL) {

                global_file_table[i] = file;
                return (int)i;
            }
        }

        if (!of_extend_table()) {
            kfree(file);
            return -1;
        }
    }
}

static int of_close(const int idx) {
    file_t *file = global_file_table[idx];
    if (!file)
        return -1;
    if (--file->refcount == 0) {
        kfree(file);
        global_file_table[idx] = NULL;
        return 0;
    }
    return -1;
}

int process_fd_open(process_t* process, const char *path) {
    const int of_idx = of_open(path);

    if (of_idx<0)
        return -1;

    fd_t* fd = kmalloc(sizeof(fd_t));

    if (!fd)
        goto clean_of;

    fd->of_idx = of_idx;

    for (;;) {

        for (size_t i = 0; i < process->fd_capacity; i++) {

            if (process->fds[i] == NULL) {
                process->fds[i] = fd;
                return (int)i;
            }
        }

        if (!process_fd_extend(process)) {
            kfree(fd);
            goto clean_of;
        }
    }

clean_of:
    of_close(of_idx);
    return -1;
}

file_t * process_unpack_fd(const process_t *process, const int fd) {
    fd_t* fd_p = process->fds[fd];
    if (!fd_p)
        return NULL;

    return global_file_table[fd_p->of_idx];
}

bool process_fd_init(process_t* process)
{
    process->fd_capacity = BASE_FD_LIST_SIZE;
    process->fds = kcalloc(process->fd_capacity, sizeof(fd_t*));

    return process->fds != 0;
}