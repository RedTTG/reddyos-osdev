#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

static long sys_open(const char* path, int flags, int mode)
{
    long res;
    __asm__ volatile (
        "syscall\n"
        : "=a"(res)
        : "a"(2),
          "D"(path),
          "S"(flags),
          "d"(mode)
        : "rcx", "r11"
    );
    return res;
}
static ssize_t sys_read(int fd, char *buffer, size_t size)
{
    long res;
    __asm__ volatile (
        "syscall\n"
        : "=a"(res)
        : "a"(0),
          "D"(fd),
          "S"(buffer),
          "d"(size)
        : "rcx", "r11"
    );
    return res;
}

static long sys_term(char c)
{
    long res;
    __asm__ volatile (
        "syscall\n"
        : "=a"(res)
        : "a"(100),
          "D"((uint64_t)(uint8_t)c)
        : "rcx", "r11"
    );
    return res;
}

static void terminal_putc(const char c)
{
    sys_term(c);
}

static void terminal_write(char* s)
{
    if (s == NULL) {
        return;
    }

    while (*s != '\0') {
        terminal_putc(*s++);
    }
}

void terminal_write_u64(uint64_t value)
{
    char buffer[21];
    size_t i = 0;

    if (value == 0) {
        terminal_putc('0');
        return;
    }

    while (value > 0 && i < sizeof(buffer)) {
        buffer[i++] = (char)('0' + (value % 10));
        value /= 10;
    }

    while (i > 0) {
        terminal_putc(buffer[--i]);
    }
}
void terminal_write_u8(uint8_t value)
{
    char buffer[4];
    size_t i = 0;

    if (value == 0) {
        terminal_putc('0');
        return;
    }

    while (value > 0 && i < sizeof(buffer)) {
        buffer[i++] = (char)('0' + (value % 10));
        value /= 10;
    }

    while (i > 0) {
        terminal_putc(buffer[--i]);
    }
}

void terminal_write_hex_u64(uint64_t value)
{
    static const char digits[] = "0123456789abcdef";

    terminal_write("0x");

    for (int shift = 60; shift >= 0; shift -= 4) {
        terminal_putc(digits[(value >> shift) & 0xF]);
    }
}
void terminal_write_hex_u8(uint8_t value)
{
    static const char digits[] = "0123456789abcdef";

    terminal_write("0x");

    for (int shift = 4; shift >= 0; shift -= 4) {
        terminal_putc(digits[(value >> shift) & 0xF]);
    }
}

void _start(void)
{
    terminal_write("\nINIT BEGIN!\n");
    int fd = sys_open("/dev/fb0", 0, 0);

    if (fd < 0) {
        terminal_write("Failed to open file\n");
        goto end;
    }

    static char buffer[64];
    sys_read(fd, buffer, sizeof(buffer));

    terminal_write("Contents of fb0 back: ");
    for (int i; i < 64; i++) {
        terminal_write_hex_u8(buffer[0]);
        terminal_putc(' ');
    }
    terminal_write("\n");
end:
    for (;;) {
    }
}