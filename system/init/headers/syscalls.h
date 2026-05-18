#pragma once
#include <stdint.h>
#include <stdio.h>

inline long sys_open(const char* path, int flags, int mode)
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
inline ssize_t sys_read(int fd, char *buffer, size_t size)
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

inline long sys_term(char c)
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