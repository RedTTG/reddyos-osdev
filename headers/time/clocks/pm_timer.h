#pragma once

#define PM_TIMER_FREQ 3579545ULL


extern struct acpi_fadt* fadt;

typedef struct {
    uint16_t port;
    bool is_32bit;
} pm_timer_t;

void pm_timer_init(void);