#include "common.h"

uint8_t fpu_default_state[512] __attribute__((aligned(16))) = {0};


void fpu_init(void)
{
    uint64_t cr0, cr4;
    uint32_t mxcsr = 0x1F80;

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

    // Clear SSE state (safe baseline for the default per-thread image)
    __asm__ volatile (
        "xorps %xmm0, %xmm0\n\t"
        "xorps %xmm1, %xmm1\n\t"
        "xorps %xmm2, %xmm2\n\t"
        "xorps %xmm3, %xmm3\n\t"
        "xorps %xmm4, %xmm4\n\t"
        "xorps %xmm5, %xmm5\n\t"
        "xorps %xmm6, %xmm6\n\t"
        "xorps %xmm7, %xmm7\n\t"
        "xorps %xmm8, %xmm8\n\t"
        "xorps %xmm9, %xmm9\n\t"
        "xorps %xmm10, %xmm10\n\t"
        "xorps %xmm11, %xmm11\n\t"
        "xorps %xmm12, %xmm12\n\t"
        "xorps %xmm13, %xmm13\n\t"
        "xorps %xmm14, %xmm14\n\t"
        "xorps %xmm15, %xmm15"
    );

    __asm__ volatile ("ldmxcsr %0" :: "m"(mxcsr));

    __asm__ volatile ("fxsave %0" : "=m"(fpu_default_state) :: "memory");
}