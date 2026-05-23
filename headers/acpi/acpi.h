#pragma once
#include <stdint.h>

#define SIGNATURE(x) (*(uint32_t*)(x))

typedef struct {
    char signature[8];
    uint8_t checksum;
    char oem_id[6];
    uint8_t revision;
    uint32_t rsdt_address;
} __attribute__ ((packed)) rsdp_t;

// structure for revision 2 (version 2.0+)
typedef struct {
    char signature[8];
    uint8_t checksum;
    char oem_id[6];
    uint8_t revision;
    uint32_t rsdt_address;      // deprecated since version 2.0

    uint32_t length;
    uint64_t xsdt_address;
    uint8_t extended_checksum;
    uint8_t reserved[3];
} __attribute__ ((packed)) xsdp_t;

typedef struct
{
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;

    char oem_id[6];
    char oem_table_id[8];

    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
} __attribute__((packed)) acpi_sdt_header_t;

typedef struct
{
    acpi_sdt_header_t header;
    uint32_t tables[];
} __attribute__((packed)) rsdt_t;

typedef struct
{
    acpi_sdt_header_t header;
    uint64_t tables[];
} __attribute__((packed)) xsdt_t;

typedef struct
{
    acpi_sdt_header_t header;

    uint32_t lapic_address;
    uint32_t flags;
} __attribute__((packed)) madt_t;

typedef struct
{
    uint8_t type;
    uint8_t length;
} __attribute__((packed)) madt_entry_t;

typedef struct
{
    uint8_t type;
    uint8_t length;

    uint8_t ioapic_id;
    uint8_t reserved;

    uint32_t ioapic_addr;
    uint32_t gsi_base;
} __attribute__((packed)) madt_ioapic_t;

typedef struct madt_entries {
    uint64_t count;
    void** entries;
} acpi_table_entries_t;

extern uint32_t lapic_address;
extern uint32_t ioapic_address;

void acpi_init(void);