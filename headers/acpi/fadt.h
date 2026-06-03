#pragma once

extern uint32_t pm_timer_address;

void get_pm_timer_address();
u8 get_century_register();
bool fadt_is_32bit();