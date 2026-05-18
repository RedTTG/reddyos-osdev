#pragma once
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>

#include "stat.h"

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
inline ssize_t sys_read(uint fd, char *buffer, size_t size)
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
inline ssize_t sys_write(uint fd, const char *buffer, size_t size)
{
    long res;
    __asm__ volatile (
        "syscall\n"
        : "=a"(res)
        : "a"(1),
          "D"(fd),
          "S"(buffer),
          "d"(size)
        : "rcx", "r11"
    );
    return res;
}

inline long sys_fstat(uint fd, stat_t* stat) {
    long res;
    __asm__ volatile (
        "syscall\n"
        : "=a"(res)
        : "a"(5),
        "D"(fd),
        "S"(stat)
        : "rcx", "r11"
    );
    return res;
}
inline long sys_stat(const char* filename, stat_t* stat) {
    long res;
    __asm__ volatile (
        "syscall\n"
        : "=a"(res)
        : "a"(4),
        "D"(filename),
        "S"(stat)
        : "rcx", "r11"
    );
    return res;
}

inline long sys_close(uint fd)
{
    long res;
    __asm__ volatile (
        "syscall\n"
        : "=a"(res)
        : "a"(3),
          "D"(fd)
        : "rcx", "r11"
    );
    return res;
}

inline long sys_lseek(uint fd, off_t offset, int whence)
{
    long res;
    __asm__ volatile (
        "syscall\n"
        : "=a"(res)
        : "a"(8),
          "D"(fd),
          "S"(offset),
          "d"(whence)
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

inline long sys_ioctl(uint fd, uint64_t request, uint64_t arg)
{
    long res;
    __asm__ volatile (
        "syscall\n"
        : "=a"(res)
        : "a"(16),
          "D"(fd),
          "S"(request),
          "d"(arg)
        : "rcx", "r11"
    );
    return res;
}