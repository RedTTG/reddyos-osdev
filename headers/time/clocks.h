#pragma once

#include "clocks/tsc.h"
#include "clocks/hpet.h"
#include "clocks/pm_timer.h"

enum clocks {
    CLOCK_TSC,
    CLOCK_HPET,
    CLOCK_PM_TIMER,
};

#define CLOCK_MASK(CLOCK) (1 << (CLOCK))
#define CLOCK_NS(CLOCK) ((CLOCK->now() * 1000000000ULL) / CLOCK->freq)


typedef struct clock_source {
    u8 id;
    u64 (*now)(void);
    u64 freq;
    struct clock_source *next;
} clock_source_t;

// Clock sources
bool is_clock_available(u8 source_id);
clock_source_t* get_clock(u8 source_id);
clock_source_t* get_best_clock(void);

// Timekeeping
clock_source_t* register_clock_source(u8 source_id, u64 (*now)(void), u64 freq);
void clocks_init(void);
u64 clock_get_tick(void);
u64 ns_since_boot(void);

// Sleep
uint64_t clock_get_ns(clock_source_t* clock);
uint64_t clock_get_best_ns(void);
void busy_sleep_ns(clock_source_t* clock, u64 ns);
void busy_sleep_best_ns(u64 ns);
void busy_sleep_ticks(clock_source_t* clock, uint64_t ticks);