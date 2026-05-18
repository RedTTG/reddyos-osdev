#include "common.h"

typedef struct block
{
    size_t size;
    int free;
    struct block* next;
    uint64_t _reserved;
} block_t;

static block_t* head = 0;
static uint64_t heap_cursor = HEAP_START;

static void* request_pages(size_t pages)
{
    uint64_t base = heap_cursor;

    for (size_t i = 0; i < pages; i++)
    {
        uint64_t phys = (uint64_t)pmm_alloc_page();

        map_page(
            heap_cursor,
            phys,
            PAGE_WRITABLE | PAGE_NX
        );

        heap_cursor += PAGE_SIZE;
    }

    return (void*)base;
}

void* kmalloc(size_t size)
{
    size = (size + 15) & ~15ULL;

    block_t* cur = head;

    while (cur)
    {
        if (cur->free && cur->size >= size)
        {
            cur->free = 0;
            return (void*)(cur + 1);
        }
        cur = cur->next;
    }

    const size_t total = sizeof(block_t) + size;
    const size_t pages = (total + PAGE_SIZE - 1) / PAGE_SIZE;

    block_t* block = request_pages(pages);

    if (!block)
        return 0;

    block->size = pages * PAGE_SIZE - sizeof(block_t);
    block->free = 0;

    block->next = head;
    head = block;

    return (void*)(block + 1);
}

void kfree(void* ptr)
{
    if (!ptr) return;

    block_t* block = (block_t*)ptr - 1;
    block->free = 1;
}

void * kcalloc(const size_t count, const size_t size) {
    const size_t total = count * size;

    void *ptr = kmalloc(total);

    if (!ptr)
        return NULL;

    memset(ptr, 0, total);

    return ptr;
}
