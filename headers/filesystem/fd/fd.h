#pragma once
typedef struct process process_t;
# define BASE_FILE_LIST_SIZE 32
# define BASE_FD_LIST_SIZE 16

typedef struct fd {
    int of_idx;
} fd_t;

extern file_t **global_file_table;
extern size_t global_file_capacity;

void fd_init();
bool process_fd_init(process_t* process);