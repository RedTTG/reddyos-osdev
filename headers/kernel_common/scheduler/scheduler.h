#pragma once

#include "isr.h"

void scheduler_init(void);
void scheduler_add(task_t* task);
void schedule();
