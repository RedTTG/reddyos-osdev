#pragma once

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#define PAGE_MMIO (PAGE_PRESENT | PAGE_WRITABLE | PAGE_NOCACHE)

#define ALIGN_UP(x, align) ((((uint64_t) (x)) + ((align) - 1)) & ~((uint64_t) ((align) - 1)))
#define ALIGN_DOWN(x, align) (((uint64_t) (x)) & ~((uint64_t) ((align) - 1)))
#define IS_ALIGNED(x, align) (((uint64_t)(x) & ((align) - 1)) == 0)