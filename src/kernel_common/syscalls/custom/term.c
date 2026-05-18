#include "common.h"

u64 do_sys_term(const char c) {
    terminal_putc(c);
    return 0;
}