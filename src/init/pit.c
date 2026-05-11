#include "common.h"
void pit_init(const uint32_t frequency)
{
    const uint32_t divisor = 1193180 / frequency;

    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
}

void pit_prepare_sleep(const uint32_t microseconds)
{
    const uint16_t divisor =
        (1193182 * microseconds) / 1000000;

    outb(0x43, 0x30);

    outb(0x40, divisor & 0xFF);
    outb(0x40, divisor >> 8);
}

void pit_perform_sleep(void)
{
    while (!(inb(0x61) & 0x20));
}