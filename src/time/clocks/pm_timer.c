#include "common.h"

static pm_timer_t* pm_timer = NULL;

static inline uint32_t pm_timer_read_raw(void) {
    return inl(pm_timer->port);
}

static uint64_t pm_timer_read(void) {
    static uint64_t accumulated = 0;
    static uint32_t last = 0;

    uint32_t current = pm_timer_read_raw();

    uint32_t mask =
        pm_timer->is_32bit ?
        0xFFFFFFFF :
        0x00FFFFFF;

    uint32_t delta =
        (current - last) & mask;

    accumulated += delta;
    last = current;

    return accumulated;
}

void pm_timer_init(void) {
    get_pm_timer_address();

    if (unlikely(!pm_timer_address))
        return;

    pm_timer = kmalloc(sizeof(pm_timer_t));
    if (!pm_timer) {
        panic("Failed to allocate memory for PM timer");
        return;
    }

    pm_timer->port = pm_timer_address;
    pm_timer->is_32bit = fadt_is_32bit();

    register_clock_source(CLOCK_PM_TIMER, pm_timer_read, PM_TIMER_FREQ);
}