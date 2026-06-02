#include <stdint.h>

// Minimal GDT + TSS setup to provide a kernel stack (rsp0) so that when
// interrupts occur while CPL=3 the CPU switches to a known kernel stack.

struct __attribute__((packed)) tss_struct {
    uint32_t reserved0;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved1;
    uint64_t ist[7];
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t iomap_base;
};

static struct tss_struct tss;

// Per-CPU kernel interrupt stack
__attribute__((aligned(16))) static uint8_t kernel_irq_stack[8192];

// Emergency stack for double faults / catastrophic transition failures.
__attribute__((aligned(16))) static uint8_t double_fault_ist_stack[8192];

void gdt_install(void)
{
    // Predefined GDT entries:
    // 0x00 null, 0x08 kernel code, 0x10 kernel data,
    // 0x18 user code, 0x20 user data, 0x28 user code (SYSRET CS),
    // 0x30/0x38 TSS.
    static uint64_t gdt_entries[8];

    gdt_entries[0] = 0x0000000000000000ULL; // null
    gdt_entries[1] = 0x00af9a000000ffffULL; // kernel code
    gdt_entries[2] = 0x00af92000000ffffULL; // kernel data
    gdt_entries[3] = 0x00affb000000ffffULL; // user code (ring3)
    gdt_entries[4] = 0x00aff3000000ffffULL; // user data (ring3)
    gdt_entries[5] = 0x00affb000000ffffULL; // user code (ring3) at 0x2b for SYSRET path

    // Initialize TSS: set rsp0 to top of our kernel_irq_stack
    for (int i = 0; i < (int)sizeof(tss); i++) ((uint8_t*)&tss)[i] = 0;
    tss.rsp0 = (uint64_t)(kernel_irq_stack + sizeof(kernel_irq_stack));
    tss.ist[0] = (uint64_t)(double_fault_ist_stack + sizeof(double_fault_ist_stack));

    // Build TSS descriptor (uses two entries)
    uint64_t tss_base = (uint64_t)&tss;
    uint32_t tss_limit = sizeof(tss) - 1;

    uint64_t desc_low = 0;
    desc_low |= (uint64_t)(tss_limit & 0xFFFF);
    desc_low |= (uint64_t)(tss_base & 0xFFFFFF) << 16;
    desc_low |= (uint64_t)0x89ULL << 40; // present, type=9 (available 64-bit TSS)
    desc_low |= (uint64_t)((tss_limit >> 16) & 0xF) << 48;
    desc_low |= (uint64_t)((tss_base >> 24) & 0xFF) << 56;

    uint64_t desc_high = (tss_base >> 32) & 0xFFFFFFFFULL;

    gdt_entries[6] = desc_low;
    gdt_entries[7] = desc_high;

    struct {
        uint16_t limit;
        uint64_t base;
    } __attribute__((packed)) gdtr;

    gdtr.limit = (uint16_t)(sizeof(gdt_entries) - 1);
    gdtr.base = (uint64_t)&gdt_entries;

    // Load the new GDT
    __asm__ volatile("lgdt %0" : : "m"(gdtr));

    // Do a far return to reload CS to our new code selector (0x08)
    __asm__ volatile(
        "pushq $0x08\n"
        "lea 1f(%%rip), %%rax\n"
        "pushq %%rax\n"
        "lretq\n"
        "1:\n"
        : : : "rax"
    );

    // Reload data segment registers with selector 0x10 (kernel data)
    __asm__ volatile(
        "mov $0x10, %%ax\n"
        "mov %%ax, %%ds\n"
        "mov %%ax, %%es\n"
        "mov %%ax, %%ss\n"
        : : : "ax"
    );

    // Load the TSS (selector 0x30)
    __asm__ volatile(
        "mov $0x30, %%ax\n"
        "ltr %%ax\n"
        : : : "ax"
    );
}

