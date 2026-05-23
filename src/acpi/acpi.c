#include "common.h"

uint32_t lapic_address = 0;
uint32_t ioapic_address = 0;

rsdp_t* acpi_get_rsdp(void)
{
    if (!rsdp_request.response)
        return 0;

    return rsdp_request.response->address;
}

acpi_table_entries_t* acpi_get_tables()
{
    rsdp_t* rsdp = acpi_get_rsdp();

    if (!rsdp)
        return 0;

    // ACPI 2.0+
    if (rsdp->revision >= 2)
    {
        const xsdp_t* xsdp = (xsdp_t*)rsdp;
        const xsdt_t* xsdt = (xsdt_t*)memvirt(xsdp->xsdt_address);

        const int entries =
            (xsdt->header.length - sizeof(acpi_sdt_header_t))
            / sizeof(uint64_t);

        acpi_table_entries_t* tables = kmalloc(sizeof(acpi_table_entries_t) + entries * sizeof(acpi_sdt_header_t*));
        tables->count = entries;
        tables->entries = (void*)(tables + 1);

        for (int i = 0; i < entries; i++)
        {
            acpi_sdt_header_t* hdr =
                (acpi_sdt_header_t*) memvirt((uint64_t)xsdt->tables[i]);

            tables->entries[i] = hdr;
        }
        return tables;
    }
    else
    {
        const rsdt_t* rsdt = (rsdt_t*) memvirt(rsdp->rsdt_address);

        const int entries =
            (rsdt->header.length - sizeof(acpi_sdt_header_t))
            / sizeof(uint32_t);

        acpi_table_entries_t* tables = kmalloc(sizeof(acpi_table_entries_t) + entries * sizeof(acpi_sdt_header_t*));
        tables->count = entries;
        tables->entries = (void*)(tables + 1);

        for (int i = 0; i < entries; i++)
        {
            acpi_sdt_header_t* hdr = (acpi_sdt_header_t*) memvirt(rsdt->tables[i]);

            tables->entries[i] = hdr;
        }
        return tables;
    }

    return 0;
}

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
    // Get the MADT table
    acpi_table_entries_t* tables = acpi_get_tables();
    if (!tables)
        goto skip_acpi;
    for (int i = 0; i < tables->count; i++) {
        acpi_sdt_header_t* entry = tables->entries[i];
        if (!memcmp(entry->signature, "APIC", 4)) {
            madt_t* madt = (madt_t*)entry;
            if (!lapic_get_msr()) {
                // Trust the madt if the MSR does not have the lapic address
                lapic_address = madt->lapic_address;
            }
            read_madt(madt);
        }
        else {
            // char* sig = kmalloc(5);
            // memcpy(sig, entry->signature, 4);
            // terminal_write("ACPI ENTRIES ");
            // terminal_write_u64(i);
            // terminal_write(" : ");
            // terminal_write(sig);
            // terminal_write("\n");
            // kfree(sig);
        }
    }
skip_acpi:
    if (!lapic_address || !ioapic_address) {
        panic("APIC not found");
    }
}