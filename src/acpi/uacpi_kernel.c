#include "common.h"
#include <uacpi/uacpi.h>
#include <uacpi/kernel_api.h>

// UNCOMMENT TO ENABLE LOGS
#define UACPII_LOG

typedef struct {
    uacpi_interrupt_handler handler;
    uacpi_handle ctx;
    uint8_t irq;
} uacpi_irq_wrapper;

static uacpi_irq_wrapper* uacpi_wrappers[16] = {0};

// ============================================================================
// RSDP Retrieval
// ============================================================================

uacpi_status uacpi_kernel_get_rsdp(uacpi_phys_addr *out_rsdp_address)
{
    if (!rsdp_request.response)
        return UACPI_STATUS_NOT_FOUND;

    rsdp_t* rsdp = (rsdp_t*)memphys((uint64_t)rsdp_request.response->address);

    if (!rsdp) {
        return UACPI_STATUS_NOT_FOUND;
    }

    *out_rsdp_address = (uacpi_phys_addr)rsdp;
    return UACPI_STATUS_OK;
}

// ============================================================================
// Memory Mapping
// ============================================================================

void *uacpi_kernel_map(uacpi_phys_addr addr, uacpi_size len)
{
    // Calculate page-aligned address and offset
    uacpi_phys_addr page_addr = addr & ~(PAGE_SIZE - 1);
    uacpi_size offset = addr - page_addr;

    // Map the virtual address using the kernel paging system
    // The kernel has identity mapped all physical memory via VIRT macro
    u64 phys_addr = vmm_virt_to_phys_as(&kernel_address_space, (u64)memvirt(page_addr)); // Ensure the page is mapped
    if (!phys_addr || phys_addr != page_addr) {
        if (len > PAGE_SIZE) {
            panic("idk how to map more than one page, fix me later.");
        }
        map_page((u64)page_addr, (u64)page_addr, PAGE_MMIO);
        return (void*)(page_addr + offset);
    }
    uint64_t virt = (u64)memvirt(page_addr);

    // Return the virtual address with the original offset
    return (void*)(virt + offset);
}

void uacpi_kernel_unmap(void *addr, uacpi_size len)
{
    u64 phys_addr = vmm_virt_to_phys_as(&kernel_address_space, (u64)addr);
    if (addr == memvirt(phys_addr)) {
        return; // HHDM mapped address
    }
    unmap_page((u64)addr);
}

// ============================================================================
// Logging
// ============================================================================

void uacpi_kernel_log(uacpi_log_level level, const uacpi_char *str)
{
#ifndef UACPII_LOG
    return;
#endif
    const char *prefix = "";

    switch (level) {
    case UACPI_LOG_DEBUG:
        prefix = "[UACPI DEBUG] ";
        break;
    case UACPI_LOG_INFO:
        prefix = "[UACPI INFO] ";
        break;
    case UACPI_LOG_WARN:
        prefix = "[UACPI WARN] ";
        break;
    case UACPI_LOG_ERROR:
        prefix = "[UACPI ERROR] ";
        break;
    default:
        break;
    }

    terminal_write(prefix);
    terminal_write(str);
}

#ifdef UACPI_FORMATTED_LOGGING
void uacpi_kernel_vlog(uacpi_log_level level, const uacpi_char *fmt, uacpi_va_list args)
{
    const char *prefix = "";

    switch (level) {
    case UACPI_LOG_DEBUG:
        prefix = "[UACPI DEBUG] ";
        break;
    case UACPI_LOG_INFO:
        prefix = "[UACPI INFO] ";
        break;
    case UACPI_LOG_WARN:
        prefix = "[UACPI WARN] ";
        break;
    case UACPI_LOG_ERROR:
        prefix = "[UACPI ERROR] ";
        break;
    }

    terminal_write(prefix);
    // TODO: Implement formatted logging if needed
    (void)fmt;
    (void)args;
}
#endif

// ============================================================================
// Initialization Hook
// ============================================================================

#ifdef UACPI_KERNEL_INITIALIZATION
uacpi_status uacpi_kernel_initialize(uacpi_init_level current_init_lvl)
{
    switch (current_init_lvl) {
    case UACPI_INIT_LEVEL_EARLY:
        // Early initialization - called from uacpi_initialize()
        break;
    case UACPI_INIT_LEVEL_SUBSYSTEM_INITIALIZED:
        // Called from uacpi_namespace_load()
        break;
    case UACPI_INIT_LEVEL_NAMESPACE_LOADED:
        // Called at start of uacpi_namespace_initialize()
        break;
    case UACPI_INIT_LEVEL_NAMESPACE_INITIALIZED:
        // Called at end of uacpi_namespace_initialize()
        break;
    }

    return UACPI_STATUS_OK;
}

void uacpi_kernel_deinitialize(void)
{
    // Cleanup if needed
}
#endif

// ============================================================================
// PCI Configuration Space Access
// ============================================================================

uacpi_status uacpi_kernel_pci_device_open(
    uacpi_pci_address address, uacpi_handle *out_handle)
{
    // TODO: Implement PCI device enumeration/handling
    // For now, return NOT_FOUND to indicate device doesn't exist
    (void)address;
    (void)out_handle;
    return UACPI_STATUS_NOT_FOUND;
}

void uacpi_kernel_pci_device_close(uacpi_handle handle)
{
    // TODO
    (void)handle;
}

uacpi_status uacpi_kernel_pci_read8(
    uacpi_handle device, uacpi_size offset, uacpi_u8 *value)
{
    // TODO
    (void)device;
    (void)offset;
    *value = 0xFF;
    return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_pci_read16(
    uacpi_handle device, uacpi_size offset, uacpi_u16 *value)
{
    // TODO
    (void)device;
    (void)offset;
    *value = 0xFFFF;
    return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_pci_read32(
    uacpi_handle device, uacpi_size offset, uacpi_u32 *value)
{
    // TODO
    (void)device;
    (void)offset;
    *value = 0xFFFFFFFF;
    return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_pci_write8(
    uacpi_handle device, uacpi_size offset, uacpi_u8 value)
{
    // TODO
    (void)device;
    (void)offset;
    (void)value;
    return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_pci_write16(
    uacpi_handle device, uacpi_size offset, uacpi_u16 value)
{
    // TODO
    (void)device;
    (void)offset;
    (void)value;
    return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_pci_write32(
    uacpi_handle device, uacpi_size offset, uacpi_u32 value)
{
    // TODO
    (void)device;
    (void)offset;
    (void)value;
    return UACPI_STATUS_OK;
}

// ============================================================================
// SystemIO Access
// ============================================================================

uacpi_status uacpi_kernel_io_map(
    uacpi_io_addr base, uacpi_size len, uacpi_handle *out_handle)
{
    // IO ports don't need mapping, just return the base address as the handle
    (void)len;
    *out_handle = (uacpi_handle)(uintptr_t)base;
    return UACPI_STATUS_OK;
}

void uacpi_kernel_io_unmap(uacpi_handle handle)
{
    (void)handle;
}

uacpi_status uacpi_kernel_io_read8(
    uacpi_handle handle, uacpi_size offset, uacpi_u8 *out_value)
{
    uacpi_io_addr base = (uacpi_io_addr)(uintptr_t)handle;
    *out_value = inb(base + offset);
    return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_io_read16(
    uacpi_handle handle, uacpi_size offset, uacpi_u16 *out_value)
{
    uacpi_io_addr base = (uacpi_io_addr)(uintptr_t)handle;
    *out_value = inw(base + offset);
    return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_io_read32(
    uacpi_handle handle, uacpi_size offset, uacpi_u32 *out_value)
{
    uacpi_io_addr base = (uacpi_io_addr)(uintptr_t)handle;
    *out_value = inl(base + offset);
    return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_io_write8(
    uacpi_handle handle, uacpi_size offset, uacpi_u8 in_value)
{
    uacpi_io_addr base = (uacpi_io_addr)(uintptr_t)handle;
    outb(base + offset, in_value);
    return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_io_write16(
    uacpi_handle handle, uacpi_size offset, uacpi_u16 in_value)
{
    uacpi_io_addr base = (uacpi_io_addr)(uintptr_t)handle;
    outw(base + offset, in_value);
    return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_io_write32(
    uacpi_handle handle, uacpi_size offset, uacpi_u32 in_value)
{
    uacpi_io_addr base = (uacpi_io_addr)(uintptr_t)handle;
    outl(base + offset, in_value);
    return UACPI_STATUS_OK;
}

// ============================================================================
// Memory Allocation
// ============================================================================

void *uacpi_kernel_alloc(uacpi_size size)
{
    return kmalloc(size);
}

#ifdef UACPI_NATIVE_ALLOC_ZEROED
void *uacpi_kernel_alloc_zeroed(uacpi_size size)
{
    return kcalloc(1, size);
}
#endif

#ifndef UACPI_SIZED_FREES
void uacpi_kernel_free(void *mem)
{
    kfree(mem);
}
#else
void uacpi_kernel_free(void *mem, uacpi_size size_hint)
{
    (void)size_hint;
    kfree(mem);
}
#endif

// ============================================================================
// Time Functions
// ============================================================================

uacpi_u64 uacpi_kernel_get_nanoseconds_since_boot(void)
{
    return ns_since_boot();
}

void uacpi_kernel_stall(uacpi_u8 usec)
{
    busy_sleep_best_ns(usec * 1000ULL);
}

void uacpi_kernel_sleep(uacpi_u64 msec)
{
    busy_sleep_best_ns(msec * 1000000ULL);
}

// ============================================================================
// Mutex and Event Objects
// ============================================================================

// Simple mutex implementation
typedef struct {
    volatile int locked;
} uacpi_kernel_mutex;

uacpi_handle uacpi_kernel_create_mutex(void)
{
    uacpi_kernel_mutex *mutex = kmalloc(sizeof(uacpi_kernel_mutex));
    if (!mutex)
        return NULL;

    mutex->locked = 0;
    return (uacpi_handle)mutex;
}

void uacpi_kernel_free_mutex(uacpi_handle handle)
{
    kfree(handle);
}

// Simple event implementation
typedef struct {
    volatile int counter;
} uacpi_kernel_event;

uacpi_handle uacpi_kernel_create_event(void)
{
    uacpi_kernel_event *event = kmalloc(sizeof(uacpi_kernel_event));
    if (!event)
        return NULL;

    event->counter = 0;
    return (uacpi_handle)event;
}

void uacpi_kernel_free_event(uacpi_handle handle)
{
    kfree(handle);
}

// ============================================================================
// Thread ID
// ============================================================================

uacpi_thread_id uacpi_kernel_get_thread_id(void)
{
    // Return current thread ID if available, otherwise return a fixed ID
    if (current_thread)
        return (uacpi_thread_id)current_thread->tid;

    return (uacpi_thread_id)1;
}

// ============================================================================
// Interrupt State Management
// ============================================================================

uacpi_interrupt_state uacpi_kernel_disable_interrupts(void)
{
    uint64_t state;
    __asm__ volatile(
        "pushfq\n\t"
        "movq (%%rsp), %0\n\t"
        "cli\n\t"
        "addq $8, %%rsp"
        : "=r"(state)
        :
        : "memory"
    );
    return (uacpi_interrupt_state)state;
}

void uacpi_kernel_restore_interrupts(uacpi_interrupt_state state)
{
    if (state & 0x200) {  // IF flag
        __asm__ volatile("sti");
    }
}

// ============================================================================
// Mutex Operations
// ============================================================================

uacpi_status uacpi_kernel_acquire_mutex(uacpi_handle handle, uacpi_u16 timeout)
{
    uacpi_kernel_mutex *mutex = (uacpi_kernel_mutex *)handle;

    // Non-blocking attempt
    if (timeout == 0) {
        if (__sync_bool_compare_and_swap(&mutex->locked, 0, 1)) {
            return UACPI_STATUS_OK;
        }
        return UACPI_STATUS_TIMEOUT;
    }

    // Busy-wait with timeout (simplified - can be improved)
    uint64_t iterations = 0;
    uint64_t max_iterations = timeout * 1000;  // Rough estimate

    while (iterations < max_iterations) {
        if (__sync_bool_compare_and_swap(&mutex->locked, 0, 1)) {
            return UACPI_STATUS_OK;
        }
        iterations++;
    }

    return UACPI_STATUS_TIMEOUT;
}

void uacpi_kernel_release_mutex(uacpi_handle handle)
{
    uacpi_kernel_mutex *mutex = (uacpi_kernel_mutex *)handle;
    __sync_bool_compare_and_swap(&mutex->locked, 1, 0);
}

// ============================================================================
// Event Operations
// ============================================================================

uacpi_bool uacpi_kernel_wait_for_event(uacpi_handle handle, uacpi_u16 timeout)
{
    uacpi_kernel_event *event = (uacpi_kernel_event *)handle;

    // Simple busy-wait
    uint64_t iterations = 0;
    uint64_t max_iterations = timeout == 0xFFFF ? 0xFFFFFFFFFFFFFFFF : timeout * 1000;

    while (iterations < max_iterations) {
        if (event->counter > 0) {
            __sync_sub_and_fetch(&event->counter, 1);
            return UACPI_TRUE;
        }
        iterations++;
    }

    return UACPI_FALSE;
}

void uacpi_kernel_signal_event(uacpi_handle handle)
{
    uacpi_kernel_event *event = (uacpi_kernel_event *)handle;
    __sync_add_and_fetch(&event->counter, 1);
}

void uacpi_kernel_reset_event(uacpi_handle handle)
{
    uacpi_kernel_event *event = (uacpi_kernel_event *)handle;
    event->counter = 0;
}

// ============================================================================
// Firmware Request Handler
// ============================================================================

uacpi_status uacpi_kernel_handle_firmware_request(uacpi_firmware_request *req)
{
    switch (req->type) {
    case UACPI_FIRMWARE_REQUEST_TYPE_BREAKPOINT:
        terminal_write("[UACPI] Breakpoint firmware request\n");
        break;
    case UACPI_FIRMWARE_REQUEST_TYPE_FATAL:
        terminal_write("[UACPI] Fatal firmware request: code=");
        terminal_write_hex_u64(req->fatal.code);
        terminal_write(", arg=");
        terminal_write_hex_u64(req->fatal.arg);
        terminal_write("\n");
        panic("uACPI firmware fatal request");
        break;
    default:
        break;
    }

    return UACPI_STATUS_OK;
}

// ============================================================================
// Interrupt Handler Installation
// ============================================================================

typedef struct {
    uacpi_interrupt_handler handler;
    uacpi_handle ctx;
    uint32_t irq;
} uacpi_irq_handler_wrapper;

uacpi_status uacpi_kernel_install_interrupt_handler(
    uacpi_u32 irq, uacpi_interrupt_handler handler, uacpi_handle ctx,
    uacpi_handle *out_irq_handle)
{
    if (irq >= 16)
        return UACPI_STATUS_INVALID_ARGUMENT;

    uacpi_irq_wrapper *w = kmalloc(sizeof(uacpi_irq_wrapper));
    if (!w)
        return UACPI_STATUS_OUT_OF_MEMORY;

    w->handler = handler;
    w->ctx = ctx;
    w->irq = irq;

    uacpi_wrappers[irq] = w;

    pic_unmask_irq(irq);
    irq_register_handler(32 + irq, (irq_handler_t)handler);

    *out_irq_handle = (uacpi_handle)w;
    return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_uninstall_interrupt_handler(
    uacpi_interrupt_handler handler, uacpi_handle irq_handle)
{
    (void)handler;
    kfree(irq_handle);
    return UACPI_STATUS_OK;
}

// ============================================================================
// Spinlock Operations
// ============================================================================

typedef struct {
    volatile int locked;
} uacpi_kernel_spinlock;

uacpi_handle uacpi_kernel_create_spinlock(void)
{
    uacpi_kernel_spinlock *spinlock = kmalloc(sizeof(uacpi_kernel_spinlock));
    if (!spinlock)
        return NULL;

    spinlock->locked = 0;
    return (uacpi_handle)spinlock;
}

void uacpi_kernel_free_spinlock(uacpi_handle handle)
{
    kfree(handle);
}

uacpi_cpu_flags uacpi_kernel_lock_spinlock(uacpi_handle handle)
{
    uacpi_kernel_spinlock *spinlock = (uacpi_kernel_spinlock *)handle;

    // Disable interrupts and save state
    uacpi_cpu_flags flags = uacpi_kernel_disable_interrupts();

    // Busy-wait for the lock
    while (__sync_bool_compare_and_swap(&spinlock->locked, 0, 1)) {
        __asm__ volatile("pause");
    }

    return flags;
}

void uacpi_kernel_unlock_spinlock(uacpi_handle handle, uacpi_cpu_flags flags)
{
    uacpi_kernel_spinlock *spinlock = (uacpi_kernel_spinlock *)handle;

    // Release the lock
    __sync_bool_compare_and_swap(&spinlock->locked, 1, 0);

    // Restore interrupts
    uacpi_kernel_restore_interrupts(flags);
}

// ============================================================================
// Deferred Work Scheduling
// ============================================================================

typedef struct {
    uacpi_work_type type;
    uacpi_work_handler handler;
    uacpi_handle ctx;
} uacpi_work_item;

#define MAX_WORK_ITEMS 256
static uacpi_work_item work_queue[MAX_WORK_ITEMS];
static volatile int work_queue_head = 0;
static volatile int work_queue_tail = 0;

uacpi_status uacpi_kernel_schedule_work(
    uacpi_work_type type, uacpi_work_handler handler, uacpi_handle ctx)
{
    // Simple work queue implementation
    int next_tail = (work_queue_tail + 1) % MAX_WORK_ITEMS;

    if (next_tail == work_queue_head) {
        // Queue is full
        return UACPI_STATUS_OUT_OF_MEMORY;
    }

    work_queue[work_queue_tail].type = type;
    work_queue[work_queue_tail].handler = handler;
    work_queue[work_queue_tail].ctx = ctx;

    work_queue_tail = next_tail;

    return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_wait_for_work_completion(void)
{
    // Process all pending work items
    while (work_queue_head != work_queue_tail) {
        uacpi_work_item item = work_queue[work_queue_head];
        work_queue_head = (work_queue_head + 1) % MAX_WORK_ITEMS;

        // Execute the work handler
        if (item.handler) {
            item.handler(item.ctx);
        }
    }

    return UACPI_STATUS_OK;
}

