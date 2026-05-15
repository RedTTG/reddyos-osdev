#include "common.h"

static uint8_t bitmap[MAX_PAGES / 8];

static void set_bit(const size_t i) {
    bitmap[i / 8] |= (1 << (i % 8));
}

static void clear_bit(const size_t i) {
    bitmap[i / 8] &= ~(1 << (i % 8));
}

static int test_bit(const size_t i) {
    return bitmap[i / 8] & (1 << (i % 8));
}

void *pmm_alloc_page(void) {
    for (size_t i = 0; i < MAX_PAGES; i++) {
        if (!test_bit(i)) {
            set_bit(i);
            return (void *) (i * PAGE_SIZE);
        }
    }
    return 0;
}

void pmm_free_page(void *page) {
    size_t i = (uint64_t) page / PAGE_SIZE;
    clear_bit(i);
}

static void mark_range_free(const uint64_t base, const uint64_t length) {
    uint64_t start = (base + PAGE_SIZE - 1) / PAGE_SIZE;
    uint64_t end = (base + length) / PAGE_SIZE;

    if (end > MAX_PAGES)
        end = MAX_PAGES;

    for (uint64_t i = start; i < end; i++)
        clear_bit(i);
}


void pmm_init(void) {
    memset(bitmap, 0xFF, sizeof(bitmap));

    // Free only the usable regions reported by Limine.
    if (memmap_request.response) {
        for (uint64_t i = 0; i < memmap_request.response->entry_count; i++) {
            const struct limine_memmap_entry *entry = memmap_request.response->entries[i];

            if (entry->type == LIMINE_MEMMAP_USABLE)
                mark_range_free(entry->base, entry->length);
        }
    }

    // Never hand out the zero page.
    set_bit(0);
}
