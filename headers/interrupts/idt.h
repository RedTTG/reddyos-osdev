#pragma once
#include <stdint.h>

extern void* isr_stub_table[];

// Define the 64bit Interrupt descriptor table struct

typedef struct
{
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t type_attributes;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero;
} __attribute__((packed)) idt_entry_t;

// Define the

typedef struct
{
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) idtr_t;

void idt_set_gate(int vector, void* isr, uint8_t flags);
void idt_init(void);