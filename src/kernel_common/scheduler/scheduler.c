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

    if (!next || next == prev) {
        return;
    }

    const uint64_t target_cr3 = next->process
        ? next->process->address_space.cr3
        : paging_kernel_cr3();

    if (target_cr3 && target_cr3 != paging_current_cr3()) {
        paging_load_cr3(target_cr3);
    }
    assert(paging_current_cr3() == target_cr3);
    assert(next != prev);
    assert(next != 0);
    assert(next->rsp != 0);
    assert(next->entry != 0);
    switch_thread_fs(next); // FS base

    current_thread = next;

    // terminal_write("Switching to thread ");
    // terminal_write_u64(current_thread->tid);
    // terminal_write(" rsp: ");
    // terminal_write_hex_u64(current_thread->rsp);
    // if (current_thread->process) {
    //     terminal_write(" of PID ");
    //     terminal_write_u64(current_thread->process->pid);
    // }
    // terminal_write("\n");

    arch_switch_thread(
        prev,
        current_thread
    );
    panic("Returned from arch_switch_thread, which should never happen");
}