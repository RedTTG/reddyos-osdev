#pragma once

// -----------------------------
// LAPIC Registers
// -----------------------------

#define LAPIC_ID               0x020
#define LAPIC_TPR              0x080
#define LAPIC_EOI              0x0B0
#define LAPIC_SVR              0x0F0

// -----------------------------
// LAPIC Timer Registers
// -----------------------------

#define LAPIC_TIMER_LVT        0x320
#define LAPIC_TIMER_INITCNT    0x380
#define LAPIC_TIMER_CURRCNT    0x390
#define LAPIC_TIMER_DIV        0x3E0

// -----------------------------
// LAPIC Timer Flags
// -----------------------------

#define LAPIC_TIMER_VECTOR     32

#define LAPIC_LVT_INT_MASKED   (1 << 16)
#define LAPIC_LVT_PERIODIC     (1 << 17)

// -----------------------------
// LAPIC Timer Dividers
// -----------------------------

#define LAPIC_DIVIDE_BY_2      0x0
#define LAPIC_DIVIDE_BY_4      0x1
#define LAPIC_DIVIDE_BY_8      0x2
#define LAPIC_DIVIDE_BY_16     0x3
#define LAPIC_DIVIDE_BY_32     0x8
#define LAPIC_DIVIDE_BY_64     0x9
#define LAPIC_DIVIDE_BY_128    0xA
#define LAPIC_DIVIDE_BY_1      0xB

// Thread time / tick rate of the timer
#define LAPIC_TIMER_MS 50

// LAPIC MS
#define IA32_APIC_BASE_MSR          0x1B
#define IA32_APIC_ENABLE            (1ULL << 11)
#define IA32_APIC_BSP               (1ULL << 8)
#define IA32_APIC_BASE_MASK         0xFFFFFFFFFFFFF000ULL

void lapic_init();
void lapic_enable(void);
void lapic_eoi(void);
bool lapic_is_enabled(void);
void lapic_timer_start(void);
bool lapic_get_msr(void);

extern bool lapic_enabled;
