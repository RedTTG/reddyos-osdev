#include "common.h"

static uint32_t* lapic;
bool lapic_enabled = false;

static void lapic_write(uint32_t reg, uint32_t val)
{
    lapic[reg / 4] = val;
}

static uint32_t lapic_read(uint32_t reg)
{
    return lapic[reg / 4];
}



static void lapic_map() {
    map_page(
        (uint64_t)memvirt(lapic_address),
        lapic_address,
        PAGE_MMIO
    );
}

void lapic_init()
{
    get_lapic_address();
    if (!lapic_address) {
        panic("Could not get LAPIC address");
        return;
    }
    lapic_map();
    lapic = (uint32_t*)memvirt(lapic_address);
    lapic_enable();
    lapic_write(LAPIC_TPR, 0);
}

void lapic_enable(void)
{
    lapic[LAPIC_SVR / 4] |= 0x100;
    lapic_enabled = true;
}



void lapic_eoi(void)
{
    lapic_write(LAPIC_EOI, 0);
}

bool lapic_is_enabled(void)
{
    lapic_enabled = lapic[LAPIC_SVR / 4] & 0x100;
    return lapic_enabled;
}

void lapic_timer_start(void)
{
    // Lower TPR so timer IRQ can pass
    lapic_write(LAPIC_TPR, 0);

    // Stop timer first
    lapic_write(
        LAPIC_TIMER_LVT,
        LAPIC_LVT_INT_MASKED
    );

    // Divider = 16
    lapic_write(
        LAPIC_TIMER_DIV,
        LAPIC_DIVIDE_BY_16
    );

    // Start LAPIC countdown from max
    lapic_write(
        LAPIC_TIMER_INITCNT,
        0xFFFFFFFF
    );

    // PIT calibration sleep (10ms)
    busy_sleep_best_ns(LAPIC_TIMER_MS * 1000000ULL);

    // Stop timer
    lapic_write(
        LAPIC_TIMER_LVT,
        LAPIC_LVT_INT_MASKED
    );

    // Calculate elapsed ticks
    uint32_t ticks_in_10ms =
        0xFFFFFFFF -
        lapic_read(LAPIC_TIMER_CURRCNT);

    // Program periodic timer
    lapic_write(
        LAPIC_TIMER_DIV,
        LAPIC_DIVIDE_BY_16
    );

    lapic_write(
        LAPIC_TIMER_LVT,
        LAPIC_TIMER_VECTOR |
        LAPIC_LVT_PERIODIC
    );

    lapic_write(
        LAPIC_TIMER_INITCNT,
        ticks_in_10ms
    );
}

bool lapic_get_msr(void)
{
    uint64_t msr = rdmsr(IA32_APIC_BASE_MSR);

    // Enable globally if needed
    if (!(msr & IA32_APIC_ENABLE))
    {
        msr |= IA32_APIC_ENABLE;
        wrmsr(IA32_APIC_BASE_MSR, msr);
    }

    lapic_address = (uint32_t)(msr & IA32_APIC_BASE_MASK);

    return true;
}