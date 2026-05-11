#include "common.h"

task_t* current_task = 0;
task_t* task_list = 0;

void scheduler_init(void) {
    static task_t bootstrap_task = {0};

    current_task = &bootstrap_task;
}

extern void context_switch(
    context_t* old,
    context_t* new
);

void scheduler_add(task_t* task)
{
    if (!task_list)
    {
        task_list = task;
        return;
    }

    task_t* iter = task_list;

    while (iter->next)
        iter = iter->next;

    iter->next = task;
}

void schedule()
{
    if (!current_task)
        return;

    task_t* prev = current_task;
    task_t* next = current_task->next ? current_task->next : task_list;

    if (!next || next == prev)
        return;

    // terminal_write("prev: ");
    // terminal_write_u64(prev->id);
    // terminal_write(", next: ");
    // terminal_write_u64(next->id);
    // terminal_write("\n");

    current_task = next;

    // terminal_write("Switching to task ");
    // terminal_write_u64(current_task->id);
    // terminal_write("\n");
    context_switch(
        &prev->context,
        &current_task->context
    );
}