#pragma once

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#define PAGE_MMIO (PAGE_PRESENT | PAGE_WRITABLE | PAGE_NOCACHE)