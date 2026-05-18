#include "syscalls.h"
#include "terminal.h"

void _start(void)
{
    terminal_write("\nINIT BEGIN!\n");
    terminal_write("\nTesting SYSCALLS\n");
    // sys open
    int fd = sys_open("test.txt", 0, 0);
    if (fd < 0) {
        terminal_write("Failed to open test.txt\n");
        goto end;
    }
    terminal_write("open -> fd: ");
    terminal_write_u8(fd);
    terminal_write("\n");
    // sys fstat
end:
    for (;;) {
    }
}