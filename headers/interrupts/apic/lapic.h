#pragma once

#define LAPIC_TMR_LVT   0x320
#define LAPIC_TMR_INIT  0x380
#define LAPIC_TMR_CURR  0x390
#define LAPIC_TMR_DIV   0x3E0
#define LAPIC_TIMER_VECTOR 32
#define LAPIC_ID       0x20
#define LAPIC_EOI      0xB0
#define LAPIC_SVR      0xF0

void lapic_map();
void lapic_init();
void lapic_enable(void);
void lapic_eoi(void);
bool lapic_is_enabled(void);

extern bool lapic_enabled;
