#pragma once
// This is the one header that includes all other common headers
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// ACPI
#include "acpi/acpi.h"

// INIT
#include "pit.h"

// INTERRUPTS
#include "idt.h"
#include "irq.h"
#include "isr.h"
#include "panic.h"

// APIC
#include "apic/ioapic.h"
#include "apic/lapic.h"

// KERNEL COMMON
#include "limine_requests.h"
#include "memory/pmm.h"
#include "paging.h"
#include "terminal.h"

// C generics
#include "io.h"
#include "itoa.h"