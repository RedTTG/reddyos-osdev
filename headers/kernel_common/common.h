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
#include "filesystem/vfs/files.h"
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
#include "memory/vmm.h"
#include "memory/heap.h"
#include "terminal.h"
#include "elf.h"

// C generics
#include "io.h"
#include "itoa.h"

// Scheduler
#include "scheduler/process.h"
#include "scheduler/thread.h"
#include "scheduler/scheduler.h"

// Syscalls
#include "syscalls/syscalls.h"