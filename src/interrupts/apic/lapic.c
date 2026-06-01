#include "common.h"

static volatile uint32_t* lapic;
bool lapic_enabled = false;

static inline void lapic_write(uint32_t reg, uint32_t val)
{
    __asm__ volatile (
        "movl %1, (%0)"
        :
        : "r" (lapic + (reg / 4)), "r" (val)
        : "memory"
    );
}

static inline uint32_t lapic_read(uint32_t reg)
{
    uint32_t val;
    __asm__ volatile (
        "movl (%1), %0"
        : "=r" (val)
        : "r" (lapic + (reg / 4))
        : "memory"
    );
    return val;
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
    lapic_check_msr();
    get_lapic_address();
    if (!lapic_address) {
        panic("Could not get LAPIC address");
        return;
    }
    terminal_write("LAPIC address: ");
    terminal_write_hex_u64(lapic_address);
    terminal_write("\n");
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

void check_lapic_timer_features(void)
{
    // uint32_t version = lapic_read(0x30) & 0xFF;
    // if (version < 0x10)
    //     panic("LAPIC timer not supported: version too old");
}

void lapic_timer_start(void)
{
    check_lapic_timer_features();
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
    busy_sleep_ns(get_clock(CLOCK_HPET), LAPIC_TIMER_MS * 1000000ULL);

    // Stop timer
    lapic_write(
        LAPIC_TIMER_LVT,
        LAPIC_LVT_INT_MASKED
    );

    // Calculate elapsed ticks
    uint32_t ticks_in_10ms =
        0xFFFFFFFF -
        lapic_read(LAPIC_TIMER_CURRCNT);

    if (ticks_in_10ms == 0)
        panic("LAPIC timer calibration failed: no ticks elapsed");

    // Program periodic timer
    lapic_write(
        LAPIC_TIMER_DIV,
        LAPIC_DIVIDE_BY_16
    );

    lapic_write(LAPIC_TIMER_LVT,
        LAPIC_TIMER_VECTOR |
        LAPIC_LVT_PERIODIC |
        LAPIC_LVT_INT_UNMASKED |
        LAPIC_DELIVERY_FIXED
    );

    lapic_write(
        LAPIC_TIMER_INITCNT,
        ticks_in_10ms
    );
}

bool lapic_check_msr(void)
{
    uint64_t msr = rdmsr(IA32_APIC_BASE_MSR);

    // Enable globally if needed
    if (!(msr & IA32_APIC_ENABLE))
    {
        msr |= IA32_APIC_ENABLE;
        wrmsr(IA32_APIC_BASE_MSR, msr);
    }

    // Disable x2APIC if enabled, since we want to use memory-mapped mode
    if (msr & IA32_APIC_X2APIC_ENABLE) {
        terminal_write("Warning: x2APIC mode enabled, disabling for memory-mapped LAPIC\n");
        msr &= ~IA32_APIC_X2APIC_ENABLE;
        wrmsr(IA32_APIC_BASE_MSR, msr);
    }

    lapic_address = (uint32_t)(msr & IA32_APIC_BASE_MASK);

    return true;
}