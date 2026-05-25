#include "common.h"
#include <uacpi/uacpi.h>
#include <uacpi/event.h>

uint32_t lapic_address = 0;
uint32_t ioapic_address = 0;

void read_madt(madt_t* madt) {
    uint8_t* ptr =
        (uint8_t*)madt + sizeof(madt_t);

    const uint8_t* end =
        (uint8_t*)madt + madt->header.length;

    while (ptr < end)
    {
        const madt_entry_t* entry =
            (madt_entry_t*)ptr;

        switch (entry->type)
        {
            case 1: {
                const madt_ioapic_t* io =
                    (madt_ioapic_t*)entry;

                ioapic_address = io->ioapic_addr;
                break;
            }
            default:
                break;
        }

        ptr += entry->length;
    }
}

void acpi_init(void)
{
    // Initialize uACPI
    uacpi_status ret = uacpi_initialize(0);
    if (uacpi_unlikely_error(ret)) {
        panic("Failed to initialize uACPI");
    }

    // Load AML namespace
    ret = uacpi_namespace_load();
    if (uacpi_unlikely_error(ret)) {
        panic("Failed to load AML namespace");
    }

    // Initialize AML namespace
    ret = uacpi_namespace_initialize();
    if (uacpi_unlikely_error(ret)) {
        panic("Failed to initialize AML namespace");
    }

    // Finish GPE initialization
    ret = uacpi_finalize_gpe_initialization();
    if (uacpi_unlikely_error(ret)) {
        panic("uACPI GPE initialization error");
    }
}