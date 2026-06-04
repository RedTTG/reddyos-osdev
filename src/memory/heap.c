#include "common.h"

#define BLOCK_MAGIC 0x52454444594F5348ULL

#ifdef KMALLOC_CANARY
typedef struct guard_block
{
    uint64_t magic;
    size_t size;
    size_t pages;
    struct guard_block* next;
} guard_block_t;

static guard_block_t* guard_head = 0;

static inline bool is_guard_block_ptr(const void* ptr)
{
    return ptr && (((uint64_t)ptr & (PAGE_SIZE - 1)) == 0);
}
#endif

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

#ifdef KMALLOC_CANARY
static void* request_guard_pages(size_t size)
{
    const size_t usable_pages = (size + PAGE_SIZE - 1) / PAGE_SIZE;
    const size_t total_pages = usable_pages + 3; // header + guard + usable + guard

    // Allocate a contiguous virtual region for metadata + guards + usable.
    void* base_virt = request_pages(total_pages);
    if (!base_virt)
        return NULL;

    uint8_t* base = (uint8_t*)base_virt;
    uint8_t* header_page = base;
    uint8_t* guard_low = base + PAGE_SIZE;
    uint8_t* usable = base + 2 * PAGE_SIZE;
    uint8_t* guard_high = base + 2 * PAGE_SIZE + usable_pages * PAGE_SIZE;

    // Unmap guard pages by clearing their mappings in the kernel address space.
    unmap_page((uint64_t)guard_low);
    unmap_page((uint64_t)guard_high);

    guard_block_t* block = (guard_block_t*)header_page;
    block->magic = BLOCK_MAGIC;
    block->size = usable_pages * PAGE_SIZE;
    block->pages = total_pages;
    block->next = guard_head;
    guard_head = block;

    return usable;
}
#endif

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

#ifdef KMALLOC_CANARY
    return request_guard_pages(size);
#endif

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

#ifdef KMALLOC_CANARY
    if (is_guard_block_ptr(ptr)) {
        guard_block_t* guard = (guard_block_t*)((uint8_t*)ptr - 2 * PAGE_SIZE);
        if (guard->magic == BLOCK_MAGIC) {
            guard->magic = 0;
            return;
        }
    }
#endif

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

#ifdef KMALLOC_CANARY
bool kmalloc_canary_selftest(void)
{
    void* p = kmalloc(PAGE_SIZE);
    if (!p)
        return false;

    // Guarded allocation should be page-aligned and backed by metadata.
    if (((uint64_t)p & (PAGE_SIZE - 1)) != 0)
        return false;

    // We do not free here because guarded allocations are intentionally simple
    // and kfree is currently a no-op for them.
    return true;
}
#endif

