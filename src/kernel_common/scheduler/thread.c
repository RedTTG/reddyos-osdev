#include  "common.h"

static uint64_t next_tid = 1;

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

void setup_kernel_stack(thread_t* thread, void (*entry)(void* arg), void* arg) {
    uint64_t* stack = (uint64_t*)(thread->kernel_stack + PAGE_SIZE);

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

    thread->rsp = (uint64_t)stack;
}

void setup_user_stack(thread_t* thread, void (*entry)(void*)) {
    uint64_t* stack = (uint64_t*)(thread->kernel_stack + PAGE_SIZE);

    // instruction pointer for RET
    *(--stack) = (uint64_t)thread->user_stack;
    *(--stack) = (uint64_t)entry;
    // thread_entry_user ^
    *(--stack) = (uint64_t)thread_entry_user;
    *(--stack) = 0;
    *(--stack) = 0;
    *(--stack) = 0;
    *(--stack) = 0;
    *(--stack) = 0;
    *(--stack) = 0;
    terminal_write("User stack: ");
    terminal_write_hex_u64((uint64_t)thread->user_stack);
    terminal_write(" tr: ");
    terminal_write_hex_u64((uint64_t)vmm_translate(&thread->process->address_space,(uint64_t)thread->user_stack));
    terminal_write("\n");
    thread->rsp = (uint64_t)stack;
}

thread_t* thread_create_base(void (*entry)(void* arg))
{
    thread_t* thread = kmalloc(sizeof(thread_t));

    if (!thread)
        return 0;

    memset(thread, 0, sizeof(*thread));

    thread->tid = next_tid++;
    thread->kernel_stack = (uint8_t*)kmalloc(PAGE_SIZE);
    if (!thread->kernel_stack)
    {
        kfree(thread);
        return 0;
    }

    thread->entry = entry;
    thread->next = 0;

    return thread;
}

thread_t* thread_create(void (*entry)(void* arg))
{
    thread_t* thread = thread_create_base(entry);

    if (!thread)
        return 0;

    setup_kernel_stack(thread, &task_bootstrap, 0);
    return thread;
}

thread_t* user_thread_create(process_t* process) {
    thread_t* thread;

    if (!process)
        return 0;

    thread = thread_create_base((void (*)(void*))process->entry_point);

    if (!thread)
    {
        return 0;
    }

    thread->process = process;
    thread->user_stack = (void*)process->user_stack_top;
    process->main_thread = thread;
    setup_user_stack(thread, (void (*)(void*))process->entry_point);
    return thread;
}