#include  "common.h"
#define TASK_STACK_SIZE 4096

static uint64_t next_pid = 1;

extern task_t* current_task;

static __attribute__((noreturn)) void task_bootstrap(void)
{
    // terminal_write("\n[BOOTSTRAP] Task ");
    // terminal_write_u64(current_task->id);
    // terminal_write(" started with entry=");
    // terminal_write_hex_u64((uint64_t)current_task->entry);
    // terminal_write("\n");

    //__asm__ volatile("sti"); NOTE: The sti is inside switch.asm

    if (current_task->entry) {
        current_task->entry();
    }

    // TODO: Mark for deletion
    for (;;)
        __asm__ volatile("hlt");
}

task_t* task_create(void (*entry)(void))
{
    task_t* task = (task_t*)pmm_alloc_virt_page();

    task->id = next_pid++;
    task->entry = entry;

    task->stack =
        (uint8_t*)pmm_alloc_virt_page();

    uint64_t stack_top =
        (uint64_t)task->stack +
        TASK_STACK_SIZE;

    // Initialize context to all zeros
    task->context.r15 = 0;
    task->context.r14 = 0;
    task->context.r13 = 0;
    task->context.r12 = 0;
    task->context.rbp = 0;
    task->context.rbx = 0;
    task->context.rax = (uint64_t)task_bootstrap;

    // Set up stack and entry point
    // DON'T put anything on the stack initially - rsp should point to empty stack
    // When we push rip, it becomes the return address
    task->context.rsp = stack_top;

    terminal_write("\nCreated task ");
    terminal_write_u64(task->id);
    terminal_write(" entry=");
    terminal_write_hex_u64((uint64_t)task->entry);
    terminal_write("\n");

    task->next = 0;

    return task;
}