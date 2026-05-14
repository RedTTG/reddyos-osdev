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

void setup_user_stack(thread_t* thread, void (*entry)(void* arg), void* arg) {
    uint64_t* stack = (uint64_t*)(thread->kernel_stack + PAGE_SIZE);

    // instruction pointer for RET
    *(--stack) = (uint64_t)thread->user_stack;
    *(--stack) = (uint64_t)entry;
    *(--stack) = (uint64_t)arg;
    *(--stack) = (uint64_t)thread_entry_user;
    *(--stack) = 0;
    *(--stack) = 0;
    *(--stack) = 0;
    *(--stack) = 0;
    *(--stack) = 0;
    *(--stack) = 0;

    thread->rsp = (uint64_t)stack;
}

thread_t* thread_create_base(void (*entry)(void* arg))
{
    thread_t* thread = pmm_alloc_virt_page();

    thread->tid = next_tid++;
    thread->kernel_stack = (uint8_t*)pmm_alloc_virt_page();
    thread->entry = entry;
    thread->next = 0;

    return thread;
}

thread_t* thread_create(void (*entry)(void* arg))
{
    thread_t* thread = thread_create_base(entry);
    setup_kernel_stack(thread, &task_bootstrap, 0);
    return thread;
}

thread_t* user_thread_create(void (*entry)(void* arg)) {
    thread_t* thread = thread_create_base(entry);
    const uint64_t user_page_virt = (uint64_t)pmm_alloc_virt_page();
    map_page(
        user_page_virt,
        (uint64_t)memphys(user_page_virt),
        PAGE_USER | PAGE_WRITABLE | PAGE_PRESENT
    );
    thread->user_stack = (void*)(user_page_virt + PAGE_SIZE);
    setup_user_stack(thread, entry, 0);
    return thread;
}