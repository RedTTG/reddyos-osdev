#include "common.h"
#include <uacpi/uacpi.h>
#include <uacpi/event.h>

void acpi_init(void)
{
    // Initialize uACPI
    uacpi_status ret = uacpi_initialize(0);
    if (uacpi_unlikely_error(ret)) {
        panic("Failed to initialize uACPI");
    }
}

void acpi_namespace_init(void) {
    // Load AML namespace
    uacpi_status ret = uacpi_namespace_load();
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