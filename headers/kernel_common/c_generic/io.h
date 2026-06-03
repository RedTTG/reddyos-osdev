#pragma once
#include <stdint.h>

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    __asm__ volatile ("inb %w1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    __asm__ volatile ("inw %w1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline uint32_t inl(uint16_t port) {
    uint32_t ret;
    __asm__ volatile ("inl %w1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile ("outb %0, %w1" : : "a"(val), "Nd"(port));
}
static inline void outw(uint16_t port, uint16_t val)
{
    __asm__ volatile ("outw %0, %w1" : : "a"(val), "Nd"(port));
}
static inline void outl(uint16_t port, uint32_t val)
{
    __asm__ volatile ("outl %0, %w1" : : "a"(val), "Nd"(port));
}


static inline void io_wait(void)
{
    __asm__ volatile ("outb %%al, $0x80" : : "a"(0));
}

static inline uint64_t rdmsr(uint32_t msr)
{
    uint32_t lo, hi;
    __asm__ volatile("rdmsr" : "=a"(lo), "=d"(hi) : "c"(msr));
    return ((uint64_t)hi << 32) | lo;
}

static inline void wrmsr(uint32_t msr, uint64_t val)
{
    uint32_t lo = (uint32_t)val;
    uint32_t hi = (uint32_t)(val >> 32);
    __asm__ volatile("wrmsr" : : "c"(msr), "a"(lo), "d"(hi));
}

static inline u8 rcmos(u8 index)
{
    u8 reg = index | 0x80;
    outb(0x70, reg);
    return inb(0x71);
}

static inline void wrmos(u8 index, u8 value)
{
    u8 reg = (index & 0x3F) | 0x80;
    outb(0x70, reg);
    outb(0x71, value);
}