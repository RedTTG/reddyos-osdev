#include "common.h"

#define ELF_MAGIC_0 0x7F
#define ELF_MAGIC_1 'E'
#define ELF_MAGIC_2 'L'
#define ELF_MAGIC_3 'F'
#define ELF_CLASS_64 2
#define ELF_DATA_LITTLE_ENDIAN 1
#define ELF_VERSION_CURRENT 1
#define ELF_MACHINE_X86_64 62

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

    // if (!(elf_flags & PF_X))
    //     flags |= PAGE_NOEXEC;

    return flags;
}

static int read_elf_header(file_t* file, elf_header_t* header)
{
    file->offset = 0;
    if (read_exact(file, header, sizeof(*header)) < 0)
        return -1;

    if (header->magic[0] != ELF_MAGIC_0 ||
        header->magic[1] != ELF_MAGIC_1 ||
        header->magic[2] != ELF_MAGIC_2 ||
        header->magic[3] != ELF_MAGIC_3)
        return -1;

    if (header->class != ELF_CLASS_64 ||
        header->data != ELF_DATA_LITTLE_ENDIAN ||
        header->version != ELF_VERSION_CURRENT ||
        header->version2 != ELF_VERSION_CURRENT ||
        header->machine != ELF_MACHINE_X86_64)
        return -1;

    if (header->phentsize != sizeof(elf_phdr_t))
        return -1;

    return 0;
}

int elf_read_info(file_t* file, elf_info_t* info)
{
    elf_header_t header;

    if (!file || !info)
        return -1;

    if (read_elf_header(file, &header) < 0)
        return -1;

    *info = (elf_info_t){
        .entry = header.entry,
        .base = 0
    };

    return 0;
}

int elf_load_into_address_space(address_space_t* as, file_t* file, const elf_info_t* info)
{
    elf_header_t header;
    uint64_t old_cr3;

    if (!as || !file || !info)
        return -1;

    if (read_elf_header(file, &header) < 0)
        return -1;

    if (info->entry != header.entry)
        return -1;

    for (uint16_t i = 0; i < header.phnum; i++) {
        elf_phdr_t ph;

        file->offset = header.phoff + ((uint64_t)i * header.phentsize);
        if (read_exact(file, &ph, sizeof(ph)) < 0)
            goto fail;

        if (ph.type != PT_LOAD)
            continue;

        if (ph.filesz > ph.memsz)
            goto fail;

        const uint64_t map_start = align_down(ph.vaddr);
        const uint64_t map_end = align_up(ph.vaddr + ph.memsz);
        const uint64_t final_flags = segment_flags(ph.flags);
        const uint64_t page_count = (map_end - map_start) >> 12;

        // terminal_write("Final flags: ");
        // if (final_flags & PAGE_WRITABLE) terminal_write("WRITABLE ");
        // if (final_flags & PAGE_USER) terminal_write("USER ");
        // if (final_flags & PAGE_WTHRU) terminal_write("WTHRU ");
        // if (final_flags & PAGE_NOCACHE) terminal_write("NOCACHE ");
        // if (final_flags & PAGE_NX) terminal_write("NX ");
        // terminal_write("\n");

        if (page_count == 0)
            continue;

        uint64_t phys_pages[page_count];

        for (uint64_t page = 0; page < page_count; page++) {
            const uint64_t virt = map_start + (page << 12);
            phys_pages[page] = (uint64_t)pmm_alloc_page();

            if (!phys_pages[page])
                goto fail;

            vmm_map(
                as,
                virt,
                phys_pages[page],
                final_flags
            );
        }

        // DEBUG: fill with 1s
        //memset((void*)map_start, 0xFF, map_end - map_start);

        file->offset = ph.offset;
        if (read_exact(file, vmm_translate(as, ph.vaddr), ph.filesz) < 0)
            goto fail;

        if (ph.memsz > ph.filesz)
            memset(vmm_translate(as, ph.vaddr + ph.filesz), 0, ph.memsz - ph.filesz);
    }

    return 0;

fail:
    return -1;
}