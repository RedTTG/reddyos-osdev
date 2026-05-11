#include "common.h"

volatile uint32_t* lapic;
bool lapic_enabled = false;

void lapic_map() {
    map_page(
        (uint64_t)memvirt(lapic_address),
        lapic_address,
        PAGE_PRESENT | PAGE_WRITABLE | PAGE_NOCACHE
    );
}

void lapic_init()
{
    lapic = (volatile uint32_t*)memvirt(lapic_address);
    lapic_enable();
}

void lapic_enable(void)
{
    lapic[LAPIC_SVR / 4] |= 0x100;
    lapic_enabled = true;
}

void lapic_write(uint32_t reg, uint32_t val)
{
    lapic[reg / 4] = val;
}

uint32_t lapic_read(uint32_t reg)
{
    return lapic[reg / 4];
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

void lapic_timer_init(uint32_t hz)
{
    // 1. Stop timer
    lapic_write(LAPIC_TMR_LVT, 0x10000);

    // 2. Set divider (divide by 16 is common)
    lapic_write(LAPIC_TMR_DIV, 0x3);

    // 3. Set periodic mode + interrupt vector
    lapic_write(LAPIC_TMR_LVT, LAPIC_TIMER_VECTOR | (1 << 17));

    // 4. Set initial count (we fake calibration for now)
    lapic_write(LAPIC_TMR_INIT, 1000000 / hz);
}