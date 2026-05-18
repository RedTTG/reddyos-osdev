#pragma once
#include <stdint.h>

static void fpu_init(void)
{
    uint64_t cr0, cr4;

    // Read CR0 / CR4
    __asm__ volatile ("mov %%cr0, %0" : "=r"(cr0));
    __asm__ volatile ("mov %%cr4, %0" : "=r"(cr4));

    // Enable FPU + SSE
    cr0 &= ~(1UL << 2);              // clear EM (no x87 emulation)
    cr0 |=  (1UL << 1);              // set MP (monitor coprocessor)

    cr4 |=  (1UL << 9);              // OSFXSR (enable SSE)
    cr4 |=  (1UL << 10);             // OSXMMEXCPT (SSE exceptions)

    __asm__ volatile ("mov %0, %%cr4" :: "r"(cr4));
    __asm__ volatile ("mov %0, %%cr0" :: "r"(cr0));

    // Initialize x87 FPU state
    __asm__ volatile ("fninit");

    // Clear SSE state (safe baseline)
    __asm__ volatile ("xorps %xmm0, %xmm0");
}