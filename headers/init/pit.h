#pragma once
#include <stdint.h>
void pit_init(uint32_t frequency);
void pit_prepare_sleep(uint32_t microseconds);
void pit_perform_sleep(void);