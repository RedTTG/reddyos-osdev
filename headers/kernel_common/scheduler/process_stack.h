#pragma once
#include <stdint.h>
#include "string.h"

#define AT_NULL     0
#define AT_PHDR     3
#define AT_PHENT    4
#define AT_PHNUM    5
#define AT_PAGESZ   6
#define AT_ENTRY    9
#define AT_UID      11
#define AT_EUID     12
#define AT_GID      13
#define AT_EGID     14
#define AT_RANDOM   25
#define AT_EXECFN   31

static inline void push_u64(uint64_t **sp, uint64_t value) {
    *--(*sp) = value;
}

static inline void push_ptr(uint64_t **sp, const void *ptr) {
    *--(*sp) = (uint64_t)ptr;
}

static inline void push_auxv(
    uint64_t **sp,
    uint64_t type,
    uint64_t value
) {
    push_u64(sp, value);
    push_u64(sp, type);
}

static char* push_string(
    uint64_t **sp,
    const char *str
) {
    size_t len = strlen(str) + 1;

    uint8_t *bytes = (uint8_t*)(*sp);

    bytes -= len;

    memcpy(bytes, str, len);

    *sp = (uint64_t*)bytes;

    return (char*)bytes;
}