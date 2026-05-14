#pragma once
// This is the one header that includes all other common headers
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "c_generic/string.h"

// ACPI
#include "acpi/acpi.h"

// Filesystem
#include "filesystem/vfs/vnode.h"
#include "filesystem/vfs/file.h"
#include "filesystem/vfs/vfs.h"
#include "filesystem/vfs/mount.h"
//
#include "filesystem/fs/tarfs/tarfs.h"

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

// Scheduler
#include "scheduler/thread.h"
#include "scheduler/scheduler.h"