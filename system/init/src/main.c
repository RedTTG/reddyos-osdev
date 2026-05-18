#include "fb.h"
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
    if (res != 0) {
        terminal_write("Failed to fstat fd\n");
        goto end;
    }
    terminal_write_stat_overview(&stat);
    // sys stat
    res = sys_stat("test.txt", &stat);
    if (res != 0) {
        terminal_write("Failed to stat filename\n");
        goto end;
    }
    terminal_write_stat_overview(&stat);
    // sys stat on dev device
    res = sys_stat("/dev/fb0", &stat);
    if (res != 0) {
        terminal_write("Failed to stat fb0\n");
        goto end;
    }
    terminal_write_stat_overview(&stat);

    long fb0 = sys_open("dev/fb0", 0, 0);
    if (fb0 < 0) {
        terminal_write("Failed to open fb0\n");
        goto end;
    }
    // ioctl
    fb_info_t fb_info;
    sys_ioctl(fb0, FB_IOCTL_GET_INFO, (uint64_t)&fb_info);
    terminal_write_fb_overview(&fb_info);
    uint8_t val[4096000];
    for (size_t y = 0; y < fb_info.height; y++) {
        for (size_t x = 0; x < fb_info.width; x++) {
            uint32_t nX = x * 255 / fb_info.width;
            uint32_t nY = y * 255 / fb_info.height;
            uint32_t idx = y * (fb_info.pitch / 4) + x;
            val[idx] = (nY << 8) | nX;
        }
    }

    res = sys_write(fb0, &val[0], sizeof(val));
    if (res <= 0) {
        terminal_write("Failed to write to fb0\n");
        goto end;
    }

    sys_ioctl(fb0, FB_IOCTL_FLIP, 0);
end:
    for (;;) {
    }
}