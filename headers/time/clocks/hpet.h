#pragma once

#define HPET_REG_CAPS      0x000
#define HPET_REG_CONFIG    0x010
#define HPET_REG_COUNTER   0x0F0

extern struct acpi_hpet* hpet;

void hpet_init(void);

// Get the HPET elapsed nanoseconds since the start of the counter
uint64_t hpet_ns(void);