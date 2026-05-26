#include "common.h"

static clock_source_t* clock_list = NULL;
static u8 available_clocks = 0;

static clock_source_t* boot_clock = NULL;
static u64 boot_clock_base = 0;

static const char* CLOCK_NAMES[] = {
    [CLOCK_TSC] = "TSC",
    [CLOCK_HPET] = "HPET",
    [CLOCK_PM_TIMER] = "PM Timer",
};

clock_source_t* get_clock_source_tail() {
    clock_source_t* current = clock_list;
    if (!current)
        return NULL;

    while (current->next)
        current = current->next;

    return current;
}

bool is_clock_available(const u8 source_id) {
    return (available_clocks & CLOCK_MASK(source_id)) != 0;
}

clock_source_t* get_clock(const u8 source_id) {
    if (unlikely(!is_clock_available(source_id)))
        return NULL; // Quick return if we don't have the clock registered
    clock_source_t* current = clock_list;
    while (current) {
        if (current->id == source_id)
            return current;
        current = current->next;
    }
    return NULL;
}

clock_source_t* get_best_clock(void) {
    if (is_clock_available(CLOCK_TSC))
        return get_clock(CLOCK_TSC);
    if (is_clock_available(CLOCK_HPET))
        return get_clock(CLOCK_HPET);
    if (is_clock_available(CLOCK_PM_TIMER))
        return get_clock(CLOCK_PM_TIMER);
    return NULL;
}

clock_source_t* register_clock_source(const u8 source_id, u64 (*now)(void), u64 freq) {
    clock_source_t* new_source = kmalloc(sizeof(clock_source_t));
    if (!new_source) {
        panic("Failed to allocate memory for clock source");
        return NULL;
    }

    available_clocks |= CLOCK_MASK(source_id);

    new_source->id = source_id;
    new_source->now = now;
    new_source->freq = freq;
    new_source->next = NULL;

    if (!clock_list) {
        clock_list = new_source;
    } else {
        clock_source_t* tail = get_clock_source_tail();
        tail->next = new_source;
    }

    return new_source;
}

void debug_avail_clocks(void) {
    clock_source_t* current = clock_list;
    terminal_write("Available clock sources:\n");
    while (current) {
        terminal_write(" - NAME: ");

        terminal_write(CLOCK_NAMES[current->id]);

        terminal_write(" (");
        terminal_write_hex_u8(current->id);
        terminal_write(")");

        terminal_write(", Freq: ");
        terminal_write_u64(current->freq);
        terminal_write(" Hz");

        terminal_write(", NOW: ");
        terminal_write_u64(current->now());
        terminal_write("\n");

        current = current->next;
    }
}

void setup_boot_clock() {
    clock_source_t* best = get_best_clock();
    if (!best)
        panic("No clock sources available");
    boot_clock = best;
    boot_clock_base = clock_get_ns(boot_clock);
}

void clocks_init(void) {
    // Clocks for calibration
    hpet_init();
    pm_timer_init();

    if (unlikely(available_clocks == 0))
        panic("No clock sources available for initial calibration");

    tsc_init();


    setup_boot_clock();
    debug_avail_clocks();
}

uint64_t ns_since_boot(void) {
    if (!boot_clock)
        panic("No boot clock!");
    return clock_get_ns(boot_clock) - boot_clock_base;
}

uint64_t clock_get_ns(clock_source_t* clock) {
    return CLOCK_NS(clock);
}

uint64_t clock_get_best_ns(void) {
    clock_source_t* best = get_best_clock();
    if (!best) {
        panic("No clock sources available");
        return 0;
    }

    return clock_get_ns(best);
}

void busy_sleep_ns(clock_source_t* clock, uint64_t ns) {
    uint64_t ticks = (ns * clock->freq) / 1000000000ULL;

    return busy_sleep_ticks(clock, ticks);
}

void busy_sleep_best_ns(uint64_t ns) {
    clock_source_t* best = get_best_clock();
    if (!best) {
        panic("No clock sources available");
        return;
    }

    busy_sleep_ns(best, ns);
}

void busy_sleep_ticks(clock_source_t* clock, uint64_t ticks) {
    uint64_t start = clock->now();
    while ((clock->now() - start) < ticks)
        __asm__ volatile ("pause");
}
