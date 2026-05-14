#include "common.h"
#include "framebuffer/basic_impl.h"

// Halt and catch fire function.
static __attribute__((noreturn)) void hcf(void)
{
    for (;;) {
        asm ("hlt");
    }
}

void timer_handler(const interrupt_frame_t* frame)
{
    (void)frame;
    // terminal_write("TIMER\n");
    schedule();
}

void task_a(void* arg)
{
    (void)arg;
    // ReSharper disable once CppDFAEndlessLoop
    while (1) {
        animate_square();
    }
}

void task_b(void* arg)
{
    (void)arg;
    process_t* process = process_create("/bin/init");
    thread_t* user_thread;

    if (!process)
        panic("Failed to create process for /bin/init");

    user_thread = user_thread_create(process);
    if (!user_thread)
        panic("Failed to create user thread for /bin/init");

    scheduler_add(user_thread);
}

void task_c(void* arg) // USER-SPACE FUNC
{
    (void)arg;
    int x = 5;
    int y = x * 10;
    (void)y;
}

void kmain(void) {
    // Ensure the bootloader actually understands our base revision (see spec).
    if (LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision) == false) {
        hcf();
    }

    terminal_init();
    terminal_write("Booted\n");


    // Ensure we got a framebuffer.
    if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1) {
        hcf();
    }

    // Fetch the first framebuffer.
    cube_init(framebuffer_request.response->framebuffers[0]);

    // Interrupts and IRQs remap
    idt_init();
    irq_init();
    irq_install();

    // ACPI
    pmm_init();
    paging_init();
    acpi_init();
    lapic_init();
    ioapic_init();
    __asm__ volatile("sti");
    // Interrupts
    ioapic_redirect_irq(0, 32); // PIT
    ioapic_redirect_irq(1, 33); // Keyboard

    irq_register_handler(32, timer_handler);

    // TarFS
    tarsf_limine_init();

    // Syscalls
    syscall_init();

    // Initialize the scheduler
    scheduler_init();

    thread_t* a = thread_create(task_a);
    thread_t* b = thread_create(task_b);

    scheduler_add(a);
    scheduler_add(b);


    // Finally start the timer
    lapic_timer_start();

    // We're done, just hang...
    hcf();
}