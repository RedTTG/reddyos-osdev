#include "syscalls.h"
#include "terminal.h"

void _start(void)
{
    terminal_write("\nINIT BEGIN!\n");
    terminal_write("\nTesting SYSCALLS\n");
    // sys open
    long fd = sys_open("test.txt", 0, 0);
    if (fd < 0) {
        terminal_write("Failed to open test.txt\n");
        goto end;
    }
    terminal_write("open -> fd: ");
    terminal_write_u8(fd);
    terminal_write("\n");
    // sys fstat
    stat_t stat;
    long res = sys_fstat(fd, &stat);
    if (res < 0) {
        terminal_write("Failed to fstat fd\n");
        goto end;
    }
    terminal_write_stat_overview(&stat);
    // sys stat
    res = sys_stat("test.txt", &stat);
    if (res < 0) {
        terminal_write("Failed to stat filename\n");
        goto end;
    }
    terminal_write_stat_overview(&stat);
    // sys stat on dev device
    res = sys_stat("/dev/fb0", &stat);
    if (res < 0) {
        terminal_write("Failed to stat fb0\n");
        goto end;
    }
    terminal_write_stat_overview(&stat);

    long fb0 = sys_open("dev/fb0", 0, 0);
    if (fb0 < 0) {
        terminal_write("Failed to open fb0\n");
        goto end;
    }
    char buffer[256];
    for (int i = 0; i < 256; i++) {
        buffer[i] = 0xFF;
    }
    res = sys_write(fb0, &buffer[0], 256);
    if (res < 0) {
        terminal_write("Failed to write to fb0\n");
        goto end;
    }
end:
    for (;;) {
    }
}