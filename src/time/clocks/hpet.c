#include <uacpi/acpi.h>
#include <uacpi/tables.h>

#include "common.h"

static volatile uint8_t* hpet_mmio;
static uint64_t hpet_period_fs;
struct acpi_hpet* hpet = NULL;

static inline uint64_t hpet_read64(uint64_t reg) {
    return *(volatile uint64_t*)(hpet_mmio + reg);
}

static inline void hpet_write64(uint64_t reg, uint64_t value) {
    *(volatile uint64_t*)(hpet_mmio + reg) = value;
}

static void find_hpet(void) {
    uacpi_table table;
    if (likely(uacpi_table_find_by_signature(ACPI_HPET_SIGNATURE, &table) == UACPI_STATUS_OK)) {
        hpet = (struct acpi_hpet*)table.ptr;
    }
}

static uint64_t hpet_read(void) {
    return hpet_read64(HPET_REG_COUNTER);
}

void hpet_init(void) {
    find_hpet();
    if (unlikely(!hpet))
        return;

    // Map the HPET to MMIO
    u64 phys = hpet->address.address;
    hpet_mmio = VIRT(phys);
    map_page((u64)hpet_mmio, phys, PAGE_MMIO);

    uint64_t caps = hpet_read64(HPET_REG_CAPS);

    // Upper 32 bits = femtoseconds per tick
    hpet_period_fs = caps >> 32;

    if (!hpet_period_fs)
        panic("Broken HPET period");

    // Disable counter
    uint64_t config = hpet_read64(HPET_REG_CONFIG);
    config &= ~1ULL;
    hpet_write64(HPET_REG_CONFIG, config);

    // Reset counter
    hpet_write64(HPET_REG_COUNTER, 0);

    // Enable counter
    config |= 1ULL;
    hpet_write64(HPET_REG_CONFIG, config);

    register_clock_source(CLOCK_HPET, hpet_read, 1000000000000000ULL / hpet_period_fs);
}