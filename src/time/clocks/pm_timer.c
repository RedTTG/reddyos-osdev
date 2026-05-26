#include <uacpi/acpi.h>
#include <uacpi/tables.h>
#include "common.h"

struct acpi_fadt* fadt = NULL;

static pm_timer_t* pm_timer = NULL;

static void find_fadt(void) {
    uacpi_table table;
    if (likely(uacpi_table_find_by_signature(ACPI_FADT_SIGNATURE, &table) == UACPI_STATUS_OK)) {
        fadt = (struct acpi_fadt*)table.ptr;
    }
}

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
    find_fadt();

    if (unlikely(!fadt))
        return;

    uacpi_u64 address = 0;

    if (fadt->x_pm_tmr_blk.address)
        address = fadt->x_pm_tmr_blk.address;
    else
        address = fadt->pm_tmr_blk;

    if (unlikely(!address))
        return;

    pm_timer = kmalloc(sizeof(pm_timer_t));
    if (!pm_timer) {
        panic("Failed to allocate memory for PM timer");
        return;
    }

    pm_timer->port = address;
    pm_timer->is_32bit = (fadt->flags & (1 << 8)) != 0;

    register_clock_source(CLOCK_PM_TIMER, pm_timer_read, PM_TIMER_FREQ);
}