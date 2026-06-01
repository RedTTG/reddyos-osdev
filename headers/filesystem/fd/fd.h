#pragma once
typedef struct process process_t;
# define BASE_FILE_LIST_SIZE 32
# define BASE_FD_LIST_SIZE 16

typedef struct fd {
    int of_idx;
} fd_t;

extern file_t **global_file_table;
extern size_t global_file_capacity;

int process_fd_open(process_t* process, const char *path, int flags, int mode);
int process_fd_close(process_t* process, int fd);
file_t* process_unpack_fd(const process_t* process, int fd);

int process_fd_base(process_t* process);

void fd_init();
bool process_fd_init(process_t* process);