#pragma once
// This is the one header that includes all other common headers

// INIT
#include "pit.h"

// INTERRUPTS
#include "apic.h"
#include "idt.h"
#include "irq.h"
#include "isr.h"
#include "panic.h"

// KERNEL COMMON
#include "limine_requests.h"
#include "terminal.h"

// C generics
#include "io.h"
#include "itoa.h"