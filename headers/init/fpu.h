#pragma once
#include <stdint.h>

extern uint8_t fpu_default_state[512] __attribute__((aligned(16)));

void fpu_init(void);