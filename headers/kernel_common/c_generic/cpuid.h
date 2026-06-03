#pragma once
#include "stdbool.h"
#include "stdint.h"


static inline void cpuid(
    uint32_t leaf, uint32_t subleaf,
    uint32_t *eax, uint32_t *ebx,
    uint32_t *ecx, uint32_t *edx) {
    __asm__ volatile (
        "cpuid"
        : "=a"(*eax),
        "=b"(*ebx),
        "=c"(*ecx),
        "=d"(*edx)
        : "a"(leaf), "c"(subleaf)
    );
}

static inline uint32_t cpuid_max_basic_leaf(void)
{
    uint32_t a, b, c, d;
    cpuid(0x0, 0, &a, &b, &c, &d);
    return a;
}

static inline uint32_t cpuid_max_ext_leaf(void)
{
    uint32_t a, b, c, d;
    cpuid(0x80000000, 0, &a, &b, &c, &d);
    return a;
}

static inline void cpuid_leaf(
    uint32_t leaf,
    uint32_t *eax, uint32_t *ebx,
    uint32_t *ecx, uint32_t *edx) {
    cpuid(leaf, 0, eax, ebx, ecx, edx);
}

static inline bool cpu_has_edx(uint32_t leaf, uint8_t bit)
{
    if (leaf > cpuid_max_basic_leaf() &&
        leaf < 0x80000000)
        return false;

    if (leaf >= 0x80000000 &&
        leaf > cpuid_max_ext_leaf())
        return false;

    uint32_t a, b, c, d;
    cpuid_leaf(leaf, &a, &b, &c, &d);

    return d & (1u << bit);
}

static inline bool cpu_has_ecx(uint32_t leaf, uint8_t bit)
{
    if (leaf > cpuid_max_basic_leaf() &&
        leaf < 0x80000000)
        return false;

    if (leaf >= 0x80000000 &&
        leaf > cpuid_max_ext_leaf())
        return false;

    uint32_t a, b, c, d;
    cpuid_leaf(leaf, &a, &b, &c, &d);

    return c & (1u << bit);
}

static inline uint8_t cpu_max_phys_bits(void)
{
    uint32_t eax, ebx, ecx, edx;
    cpuid_leaf(0x80000008, &eax, &ebx, &ecx, &edx);

    return (uint8_t)(eax & 0xFF);
}

static inline uint64_t cpu_max_phys_mask(void)
{
    uint8_t bits = cpu_max_phys_bits();

    if (bits >= 64)
        return ~0ULL;

    return (1ULL << bits) - 1;
}