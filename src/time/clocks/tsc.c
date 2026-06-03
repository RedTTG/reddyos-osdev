#include "common.h"

static u64 tsc_base;

static bool tsc_available(void) {
    return cpu_has_edx(0x1, 4);
}

static bool invariant_tsc(void) {
    return cpu_has_edx(0x80000007, 8);
}

static inline uint64_t rdtsc(void) {
    uint32_t lo, hi;

    __asm__ volatile(
        "rdtsc"
        : "=a"(lo), "=d"(hi)
    );

    return ((uint64_t)hi << 32) | lo;
}

uint64_t calibrate_tsc(void) {
    uint64_t start_tsc = rdtsc();
    busy_sleep_best_ns(100000000); // Sleep for 100ms

    uint64_t end_tsc = rdtsc();

    uint64_t elapsed = end_tsc - start_tsc;

    return elapsed * 10; // scale 100ms → Hz
}

u64 tsc_read(void) {
    return rdtsc() - tsc_base;
}

void tsc_init(void) {
    if (!tsc_available()) {
        terminal_write("WARNING: TSC not available\n");
        return;
    }
    if (invariant_tsc()) {
        terminal_write("The TSC is invariant\n");
    } else {
        terminal_write("The TSC is NOT invariant\n");
    }

    uint64_t freq = calibrate_tsc();
    tsc_base = rdtsc();
    if (freq == 0)
        return;

    register_clock_source(CLOCK_TSC, tsc_read, freq);
}