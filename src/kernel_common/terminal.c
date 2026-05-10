#include "common.h"

#include <stdbool.h>

static const uint16_t SERIAL_PORT = 0x3F8;
static bool terminal_ready = false;

static void terminal_wait(void)
{
    while ((inb(SERIAL_PORT + 5) & 0x20) == 0) {
    }
}

static void terminal_putc_raw(char c)
{
    terminal_wait();
    outb(SERIAL_PORT, (uint8_t)c);
}

void terminal_init(void)
{
    outb(SERIAL_PORT + 1, 0x00);
    outb(SERIAL_PORT + 3, 0x80);
    outb(SERIAL_PORT + 0, 0x03);
    outb(SERIAL_PORT + 1, 0x00);
    outb(SERIAL_PORT + 3, 0x03);
    outb(SERIAL_PORT + 2, 0xC7);
    outb(SERIAL_PORT + 4, 0x0B);
    terminal_ready = true;
}

void terminal_putc(char c)
{
    if (!terminal_ready) {
        terminal_init();
    }

    if (c == '\n') {
        terminal_putc_raw('\r');
    }

    terminal_putc_raw(c);
}

void terminal_write(const char *s)
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

void terminal_write_hex_u64(uint64_t value)
{
    static const char digits[] = "0123456789abcdef";

    terminal_write("0x");

    for (int shift = 60; shift >= 0; shift -= 4) {
        terminal_putc(digits[(value >> shift) & 0xF]);
    }
}
