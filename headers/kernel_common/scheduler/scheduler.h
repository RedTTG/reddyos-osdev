#pragma once

void scheduler_init(void);
void scheduler_add(thread_t* thread);
void schedule();
