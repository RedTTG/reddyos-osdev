#pragma once
// This is the one header that includes all other common headers
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include "types_extra.h"
#include "helpers_extra.h"

#include "c_generic/string.h"

// ACPI
#include "acpi/acpi.h"
#include "acpi/madt.h"
#include "acpi/fadt.h"

// Filesystem
#include "filesystem/vfs/vnode.h"
#include "filesystem/vfs/files.h"
#include "filesystem/vfs/vfs.h"
#include "filesystem/vfs/mount.h"
// Filesystems
#include "filesystem/fs/tarfs/tarfs.h"
#include "filesystem/fs/devfs/devfs.h"
// File descriptors
#include "filesystem/fd/fd.h"

// Devices
#include "framebuffer/fb.h"

// INIT
#include "fpu.h"

// CLOCKS
#include "time/clocks.h"

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
#include "memory/vma.h"
#include "memory/heap.h"
#include "terminal.h"
#include "elf.h"

// C generics
#include "io.h"
#include "itoa.h"
#include "cpuid.h"

// Scheduler
#include "scheduler/process.h"
#include "scheduler/thread.h"
#include "scheduler/scheduler.h"

// Syscalls
#include "syscalls/syscalls.h"