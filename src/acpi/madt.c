#include "common.h"
#include <uacpi/acpi.h>
#include <uacpi/tables.h>

uint32_t lapic_address = 0;
uint32_t ioapic_address = 0;

struct acpi_madt* madt = NULL;

static void find_madt() {
    if (madt)
        return;
    uacpi_table table;
    if (likely(uacpi_table_find_by_signature(ACPI_MADT_SIGNATURE, &table) == UACPI_STATUS_OK)) {
        madt = (struct acpi_madt*)table.ptr;
    }
}

static void read_madt() {
    uint8_t* ptr = (u8*)madt->entries;

    const uint8_t* end =
        (uint8_t*)madt + madt->hdr.length;

    while (ptr < end)
    {
        struct acpi_entry_hdr* entry =
            (struct acpi_entry_hdr*)ptr;

        switch (entry->type)
        {
            case ACPI_MADT_ENTRY_TYPE_IOAPIC: {
                const struct acpi_madt_ioapic* io =
                    (struct acpi_madt_ioapic*)entry;

                ioapic_address = io->address;
                break;
            }
            default:
                break;
        }

        ptr += entry->length;
    }
}

void get_lapic_address(void) {
    find_madt();
    if (!madt)
        return;
    lapic_address = madt->local_interrupt_controller_address;
}

void get_ioapic_address(void) {
    find_madt();
    if (!madt)
        return;
    read_madt();
}
