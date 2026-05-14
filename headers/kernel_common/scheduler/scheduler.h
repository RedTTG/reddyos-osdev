#pragma once

#include "isr.h"

void scheduler_init(void);
void scheduler_add(thread_t* thread);
void schedule();
