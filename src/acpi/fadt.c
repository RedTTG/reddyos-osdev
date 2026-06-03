#include "common.h"
#include <uacpi/acpi.h>
#include <uacpi/tables.h>

uint32_t pm_timer_address = 0;

struct acpi_fadt* fadt = NULL;

static void find_fadt() {
    if (fadt)
        return;
    uacpi_table table;
    if (likely(uacpi_table_find_by_signature(ACPI_FADT_SIGNATURE, &table) == UACPI_STATUS_OK)) {
        fadt = (struct acpi_fadt*)table.ptr;
    }
}

void get_pm_timer_address() {
    find_fadt();
    if (!fadt)
        return;
    if (fadt->x_pm_tmr_blk.address)
        pm_timer_address = fadt->x_pm_tmr_blk.address;
    else
        pm_timer_address = fadt->pm_tmr_blk;
}

u8 get_century_register() {
    find_fadt();
    if (!fadt)
        return 0;
    return fadt->century;
}

bool fadt_is_32bit() {
    find_fadt();
    if (!fadt)
        return false;
    return (fadt->flags & (1 << 8)) != 0;
}
