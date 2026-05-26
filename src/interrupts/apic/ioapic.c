#include "common.h"

volatile uint32_t* ioapic;

static void ioapic_map() {
    map_page(
        (uint64_t)memvirt(ioapic_address),
        ioapic_address,
        PAGE_MMIO
    );
}

void ioapic_init()
{
    ioapic_map();
    ioapic = (volatile uint32_t*)memvirt(ioapic_address);
}

static void ioapic_write(uint8_t reg, uint32_t value)
{
    ioapic[0] = reg;
    ioapic[4] = value;
}

static uint32_t ioapic_read(uint8_t reg)
{
    ioapic[0] = reg;
    return ioapic[4];
}

void ioapic_redirect_irq(uint8_t irq, uint8_t vector)
{
    uint8_t reg = 0x10 + irq * 2;

    uint64_t entry = 0;

    entry |= vector;          // interrupt vector
    entry |= (0ULL << 16);    // UNMASK
    entry |= ((uint64_t)0 << 56); // CPU 0

    ioapic_write(reg, (uint32_t)entry);
    ioapic_write(reg + 1, (uint32_t)(entry >> 32));
}