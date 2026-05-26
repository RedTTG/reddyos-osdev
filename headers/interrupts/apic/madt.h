#pragma once

extern uint32_t lapic_address;
extern uint32_t ioapic_address;

void get_lapic_address(void);
void get_ioapic_address(void);