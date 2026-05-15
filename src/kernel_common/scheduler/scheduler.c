#include "common.h"

thread_t* current_thread = 0;
thread_t* thread_list = 0;
static thread_t bootstrap_task = {0};

void scheduler_init(void) {

    current_thread = &bootstrap_task;
}

void scheduler_add(thread_t* thread)
{
    if (!thread_list)
    {
        thread_list = thread;
        // current_thread = thread_list;
        return;
    }

    thread_t* iter = thread_list;

    while (iter->next)
        iter = iter->next;

    iter->next = thread;
}

void schedule()
{
    if (!current_thread)
        return;

    thread_t* prev = current_thread;
    thread_t* next = current_thread->next ? current_thread->next : thread_list;

    if (!next || next == prev)
        return;

    const uint64_t target_cr3 = next->process
        ? next->process->address_space.cr3
        : paging_kernel_cr3();

    if (target_cr3 && target_cr3 != paging_current_cr3()) {
        // terminal_write("Switching target cr3 to: ");
        // terminal_write_hex_u64(target_cr3);
        // terminal_write("\n");
        paging_load_cr3(target_cr3);
    }

    // terminal_write("prev (");
    // terminal_write_u64(prev->tid);
    // terminal_write("): ");
    // terminal_write_hex_u64(prev->rsp);
    // terminal_write(", next (");
    // terminal_write_u64(next->tid);
    // terminal_write("): ");
    // terminal_write_hex_u64(next->rsp);
    // terminal_write("\n");

    current_thread = next;

    // if (current_thread->process) {
    //     terminal_write("Switching to thread ");
    //     terminal_write_u64(current_thread->tid);
    //     terminal_write(" of PID ");
    //     terminal_write_u64(current_thread->process->pid);
    //     terminal_write("\n");
    // }

    arch_switch_thread(
        prev,
        current_thread
    );
}