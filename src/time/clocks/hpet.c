#include <uacpi/acpi.h>
#include <uacpi/tables.h>

#include "common.h"

struct acpi_hpet* hpet = NULL;

void hpet_init(void) {
    uacpi_table table;
    if (uacpi_table_find_by_signature(ACPI_HPET_SIGNATURE, &table) == UACPI_STATUS_OK) {
        hpet = (struct acpi_hpet*)table.ptr;
    }
}