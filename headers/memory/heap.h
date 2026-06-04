#pragma once

#define KERNEL_STACK_START 0xFFFFBFFF00000000ULL
#define KERNEL_STACK_PAGES 16
#define HEAP_START 0xFFFFC00000000000ULL

#ifdef KMALLOC_CANARY
#define KMALLOC_GUARD_MODE 1
#else
#define KMALLOC_GUARD_MODE 0
#endif

void* kmalloc(size_t size);
void kfree(void* ptr);
void *kcalloc(size_t count, size_t size);

#ifdef KMALLOC_CANARY
bool kmalloc_canary_selftest(void);
#endif
