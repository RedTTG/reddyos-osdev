#include  "common.h"
#define TASK_STACK_SIZE 4096

static uint64_t next_pid = 1;

extern thread_t* current_thread;

static __attribute__((noreturn)) void task_bootstrap(void)
{
    // terminal_write("\n[BOOTSTRAP] Task ");
    // terminal_write_u64(current_task->id);
    // terminal_write(" started with entry=");
    // terminal_write_hex_u64((uint64_t)current_task->entry);
    // terminal_write("\n");

    //__asm__ volatile("sti"); NOTE: The sti is inside switch.asm

    if (current_thread->entry) {
        current_thread->entry();
    }

    terminal_write("Thread with ID=");
    terminal_write_u64(current_thread->id);
    terminal_write(" is ready for free\n");

    // TODO: Mark for deletion
    for (;;)
        __asm__ volatile("hlt");
}

thread_t* thread_create(void (*entry)(void))
{
    thread_t* thread = (thread_t*)pmm_alloc_virt_page();

    thread->id = next_pid++;
    thread->entry = entry;

    thread->stack =
        (uint8_t*)pmm_alloc_virt_page();

    uint64_t stack_top =
        (uint64_t)thread->stack +
        TASK_STACK_SIZE;

    // Initialize context to all zeros
    thread->context.r15 = 0;
    thread->context.r14 = 0;
    thread->context.r13 = 0;
    thread->context.r12 = 0;
    thread->context.rbp = 0;
    thread->context.rbx = 0;


    // Setup stack and entry
    thread->context.rsp = stack_top;
    thread->context.rax = (uint64_t)task_bootstrap;

    // terminal_write("\nCreated thread ");
    // terminal_write_u64(task->id);
    // terminal_write(" entry=");
    // terminal_write_hex_u64((uint64_t)task->entry);
    // terminal_write("\n");

    thread->next = 0;

    return thread;
}