#pragma once
#include <stdint.h>

#define IA32_APIC_BASE_MSR 0x1B
#define APIC_ENABLE (1ULL << 11)
#define APIC_X2APIC_ENABLE (1ULL << 10)
#define APIC_BASE_MASK 0xFFFFFFFFFFFFF000ULL
#define LAPIC_DEFAULT_BASE 0xFEE00000

void apic_init(void);
void apic_eoi(void);
int apic_is_enabled(void);
