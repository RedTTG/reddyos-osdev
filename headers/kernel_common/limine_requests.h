#pragma once
#include <limine.h>
#include <stdint.h>

extern volatile uint64_t limine_base_revision[];
extern volatile struct limine_framebuffer_request framebuffer_request;
extern volatile struct limine_hhdm_request hhdm_request;
extern volatile struct limine_memmap_request memmap_request;
extern volatile struct limine_rsdp_request rsdp_request;

// Helper function for hhdm physical to virtual address space
void* memvirt(uint64_t phys);
void* memphys(uint64_t virt);
