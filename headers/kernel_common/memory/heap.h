#pragma once

#define HEAP_START 0xFFFFC00000000000ULL

void* kmalloc(size_t size);
void kfree(void* ptr);
void *kcalloc(size_t count, size_t size);