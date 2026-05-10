#include "common.h"

static idt_entry_t idt[256];
static idtr_t idtr;
static uint16_t kernel_cs_selector;

static uint16_t get_kernel_cs_selector(void)
{
    uint16_t cs;
    __asm__ volatile ("mov %%cs, %0" : "=r"(cs));
    return cs;
}

void idt_set_gate(int vector, void* isr, uint8_t flags)
{
    uint64_t addr = (uint64_t)isr;

    idt[vector].offset_low = addr & 0xFFFF;
    idt[vector].selector = kernel_cs_selector;
    idt[vector].ist = 0;
    idt[vector].type_attributes = flags;
    idt[vector].offset_mid = (addr >> 16) & 0xFFFF;
    idt[vector].offset_high = (addr >> 32) & 0xFFFFFFFF;
    idt[vector].zero = 0;
}

static inline void lidt(idtr_t* idtr)
{
    __asm__ volatile("lidt %0" : : "m"(*idtr));
}

void idt_init(void)
{
    kernel_cs_selector = get_kernel_cs_selector();

    for (int i = 0; i < 32; i++)
        idt_set_gate(i, isr_stub_table[i], 0x8E);

    idtr.base = (uint64_t)&idt;
    idtr.limit = sizeof(idt) - 1;

    lidt(&idtr);
}