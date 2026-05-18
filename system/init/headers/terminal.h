#pragma once
#include "stat.h"

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

static void terminal_write_mode_string(uint32_t mode)
{
    terminal_putc((mode & S_IRUSR) ? 'r' : '-');
    terminal_putc((mode & S_IWUSR) ? 'w' : '-');
    terminal_putc((mode & S_IXUSR) ? 'x' : '-');
    terminal_putc((mode & S_IRGRP) ? 'r' : '-');
    terminal_putc((mode & S_IWGRP) ? 'w' : '-');
    terminal_putc((mode & S_IXGRP) ? 'x' : '-');
    terminal_putc((mode & S_IROTH) ? 'r' : '-');
    terminal_putc((mode & S_IWOTH) ? 'w' : '-');
    terminal_putc((mode & S_IXOTH) ? 'x' : '-');
}

static void terminal_write_file_type(uint32_t mode)
{
    switch (mode & S_IFMT) {
        case S_IFREG:
            terminal_write("regular file");
            break;
        case S_IFDIR:
            terminal_write("directory");
            break;
        case S_IFCHR:
            terminal_write("character device");
            break;
        case S_IFBLK:
            terminal_write("block device");
            break;
        case S_IFIFO:
            terminal_write("fifo/pipe");
            break;
        case S_IFLNK:
            terminal_write("symbolic link");
            break;
        default:
            terminal_write("unknown");
            break;
    }
}

static void terminal_write_stat_overview(const stat_t* stat)
{
    if (!stat) {
        terminal_write("stat: <null>\n");
        return;
    }

    terminal_write("stat overview:\n");
    terminal_write("  type: ");
    terminal_write_file_type(stat->st_mode);
    terminal_write("\n");

    terminal_write("  mode: ");
    terminal_write_mode_string(stat->st_mode);
    terminal_write(" (0x");
    terminal_write_hex_u64(stat->st_mode);
    terminal_write(")\n");

    terminal_write("  size: ");
    terminal_write_u64((uint64_t)stat->st_size);
    terminal_write(" bytes\n");

    terminal_write("  nlink: ");
    terminal_write_u64(stat->st_nlink);
    terminal_write("\n");

    terminal_write("  uid: ");
    terminal_write_u64(stat->st_uid);
    terminal_write("\n");

    terminal_write("  gid: ");
    terminal_write_u64(stat->st_gid);
    terminal_write("\n");

    terminal_write("  dev: ");
    terminal_write_hex_u64(stat->st_dev);
    terminal_write("\n");

    terminal_write("  ino: ");
    terminal_write_hex_u64(stat->st_ino);
    terminal_write("\n");

    terminal_write("  rdev: ");
    terminal_write_hex_u64(stat->st_rdev);
    terminal_write("\n");
}

