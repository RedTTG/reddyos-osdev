#include "common.h"

#define BLOCK_MAGIC 0x52454444594F5348ULL

typedef struct block
{
    uint64_t magic;
    size_t size;
    int free;
    struct block* next;
} block_t;

static block_t* head = 0;
static uint64_t heap_cursor = HEAP_START;

static void* request_pages(size_t pages)
{
    uint64_t base = heap_cursor;

    for (size_t i = 0; i < pages; i++)
    {
        uint64_t phys = (uint64_t)pmm_alloc_page();
        if (!phys) {
            return NULL;
        }

        map_page(
            heap_cursor,
            phys,
            PAGE_WRITABLE | PAGE_NX
        );

        heap_cursor += PAGE_SIZE;
    }

    // Leave an unmapped guard page between allocations so an overflow in one
    // heap object faults immediately instead of corrupting the next block's
    // metadata.
    heap_cursor += PAGE_SIZE;

    return (void*)base;
}

static inline bool block_is_valid(const block_t* block)
{
    if (!block)
        return false;

    if (((uint64_t)block & (PAGE_SIZE - 1)) != 0)
        return false;

    return block->magic == BLOCK_MAGIC;
}

void* kmalloc(size_t size)
{
    size = (size + 15) & ~15ULL;

    block_t* cur = head;

    while (cur)
    {
        if (!block_is_valid(cur))
            break;

        if (cur->free && cur->size >= size)
        {
            cur->free = 0;
            return (void*)(cur + 1);
        }

        block_t* next = cur->next;
        if (!block_is_valid(next) && next != NULL)
            break;

        cur = next;
    }

    const size_t total = sizeof(block_t) + size;
    const size_t pages = (total + PAGE_SIZE - 1) / PAGE_SIZE;

    block_t* block = request_pages(pages);

    if (!block)
        return 0;
    block->magic = BLOCK_MAGIC;
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
    if (!block_is_valid(block))
        return;

    block->free = 1;
}

void * kcalloc(const size_t count, const size_t size) {
    if (size != 0 && count > SIZE_MAX / size)
        return NULL;

    const size_t total = count * size;

    void *ptr = kmalloc(total);

    if (!ptr)
        return NULL;

    memset(ptr, 0, total);

    return ptr;
}
