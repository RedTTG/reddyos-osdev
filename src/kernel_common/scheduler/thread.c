#include  "common.h"
#define THREAD_STACK_SIZE 4096

static uint64_t next_pid = 1;

extern thread_t* current_thread;

static __attribute__((noreturn)) void task_bootstrap(void* arg)
{
    //__asm__ volatile("sti"); NOTE: The sti is inside switch.asm

    if (current_thread->entry) {
        current_thread->entry(arg);
    }

    // terminal_write("Thread with ID=");
    // terminal_write_u64(current_thread->tid);
    // terminal_write(" is ready for free\n");

    // TODO: Mark for deletion
    for (;;)
        __asm__ volatile("hlt");
}

void setup_initial_stack(thread_t* thread, void (*entry)(void* arg), void* arg) {
    uint64_t* stack = (uint64_t*)(thread->stack + THREAD_STACK_SIZE);

    // instruction pointer for RET
    *(--stack) = (uint64_t)entry;
    *(--stack) = (uint64_t)arg;
    *(--stack) = (uint64_t)thread_entry_kernel;
    *(--stack) = 0;
    *(--stack) = 0;
    *(--stack) = 0;
    *(--stack) = 0;
    *(--stack) = 0;
    *(--stack) = 0;

// RIP
// RBX
// RBP
// R12
// R13
// R14
// R15

    thread->rsp = (uint64_t)stack;
}

thread_t* thread_create(void (*entry)(void* arg))
{
    thread_t* thread = pmm_alloc_virt_page();

    thread->tid = next_pid++;

    thread->stack = (uint8_t*)pmm_alloc_virt_page();

    thread->entry = entry;

    setup_initial_stack(thread, &task_bootstrap, 0);

    thread->next = 0;

    return thread;
}