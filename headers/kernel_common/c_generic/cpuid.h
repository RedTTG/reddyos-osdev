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

static inline void cpuid_leaf(
    uint32_t leaf,
    uint32_t *eax, uint32_t *ebx,
    uint32_t *ecx, uint32_t *edx) {
    cpuid(leaf, 0, eax, ebx, ecx, edx);
}

static inline bool cpu_has(uint32_t leaf, uint8_t bit) {
    uint32_t a, b, c, d;
    cpuid_leaf(leaf, &a, &b, &c, &d);
    return d & (1u << bit);
}
