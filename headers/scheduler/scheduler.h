#pragma once

extern thread_t* thread_list;
extern thread_t* current_thread;
#define CUR_PROCESS current_thread->process

void scheduler_init(void);
void scheduler_add(thread_t* thread);
void schedule();
