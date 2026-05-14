#include "common.h"

static int read_exact(file_t* file, void* buffer, uint64_t size)
{
    uint8_t* out = buffer;

    while (size > 0) {
        const int read = vfs_read(file, out, size);

        if (read <= 0)
            return -1;

        out += read;
        size -= (uint64_t)read;
    }

    return 0;
}

static uint64_t align_down(uint64_t value)
{
    return value & ~0xFFFULL;
}

static uint64_t align_up(uint64_t value)
{
    return (value + 0xFFFULL) & ~0xFFFULL;
}

static uint64_t segment_flags(uint32_t elf_flags)
{
    uint64_t flags = PAGE_USER | PAGE_PRESENT;

    if (elf_flags & PF_W)
        flags |= PAGE_WRITABLE;

    if (!(elf_flags & PF_X))
        flags |= PAGE_NOEXEC;

    return flags;
}

elf_info_t elf_load(file_t* file)
{
    elf_header_t header;

    file->offset = 0;
    if (read_exact(file, &header, sizeof(header)) < 0)
        return (elf_info_t){0};

    for (uint16_t i = 0; i < header.phnum; i++) {
        elf_phdr_t ph;

        file->offset = header.phoff + ((uint64_t)i * header.phentsize);
        if (read_exact(file, &ph, sizeof(ph)) < 0)
            return (elf_info_t){0};

        if (ph.type != PT_LOAD)
            continue;

        if (ph.filesz > ph.memsz)
            return (elf_info_t){0};

        const uint64_t map_start = align_down(ph.vaddr);
        const uint64_t map_end = align_up(ph.vaddr + ph.memsz);
        const uint64_t final_flags = segment_flags(ph.flags);
        const uint64_t load_flags = final_flags | PAGE_WRITABLE;
        const uint64_t page_count = (map_end - map_start) >> 12;

        if (page_count == 0)
            continue;

        uint64_t phys_pages[page_count];

        for (uint64_t page = 0; page < page_count; page++) {
            const uint64_t virt = map_start + (page << 12);
            phys_pages[page] = (uint64_t)pmm_alloc_page();

            map_page(
                virt,
                phys_pages[page],
                load_flags
            );
        }

        file->offset = ph.offset;
        if (read_exact(file, (void*)ph.vaddr, ph.filesz) < 0)
            return (elf_info_t){0};

        if (ph.memsz > ph.filesz)
            memset((void*)(ph.vaddr + ph.filesz), 0, ph.memsz - ph.filesz);

        for (uint64_t page = 0; page < page_count; page++) {
            map_page(
                map_start + (page << 12),
                phys_pages[page],
                final_flags
            );
        }
    }

    return (elf_info_t){
        .entry = header.entry,
        .base = 0
    };
}