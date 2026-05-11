#pragma once
#include <stdint.h>
void ioapic_init();
void ioapic_redirect_irq(uint8_t irq, uint8_t vector);