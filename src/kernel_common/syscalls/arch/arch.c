#include "common.h"
#include "errors.h"

u64 do_sys_arch_prctl(int op, u64 addr) {
    terminal_write("sys_arch_prctl called with op: ");
    terminal_write_hex_u64(op);
    terminal_write(" and addr: ");
    terminal_write_hex_u64(addr);
    terminal_write("\n");
    return -ENOSYS; // Not implemented yet
}