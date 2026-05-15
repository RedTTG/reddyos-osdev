#include "common.h"

uint32_t lapic_address = 0;
uint32_t ioapic_address = 0;

rsdp_t* acpi_get_rsdp(void)
{
    if (!rsdp_request.response)
        return 0;

    return rsdp_request.response->address;
}

acpi_sdt_header_t* acpi_find_table(const char* sig)
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

        for (int i = 0; i < entries; i++)
        {
            acpi_sdt_header_t* hdr =
                (acpi_sdt_header_t*) memvirt((uint64_t)xsdt->tables[i]);

            if (!memcmp(hdr->signature, sig, 4))
                return hdr;
        }
    }
    else
    {
        const rsdt_t* rsdt = (rsdt_t*) memvirt(rsdp->rsdt_address);

        const int entries =
            (rsdt->header.length - sizeof(acpi_sdt_header_t))
            / sizeof(uint32_t);

        for (int i = 0; i < entries; i++)
        {
            acpi_sdt_header_t* hdr = (acpi_sdt_header_t*) memvirt(rsdt->tables[i]);

            if (!memcmp(hdr->signature, sig, 4))
                return hdr;
        }
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
    madt_t* madt = (madt_t*)acpi_find_table("APIC");
    if (!madt) {
        panic("APIC not found");
        return;
    }
    lapic_address = madt->lapic_address;
    read_madt(madt);
}