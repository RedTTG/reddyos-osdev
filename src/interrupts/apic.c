#include "common.h"
#include "apic.h"

#define APIC_EOI_MSR 0x80B
#define APIC_SVR_MSR 0x80F
#define APIC_TPR_MSR 0x808
static int apic_x2apic_enabled = 0;

static inline void wrmsr_u64(uint32_t msr, uint64_t value)
{
    uint32_t lo = (uint32_t)(value & 0xFFFFFFFF);
    uint32_t hi = (uint32_t)(value >> 32);
    __asm__ volatile ("wrmsr" : : "c"(msr), "a"(lo), "d"(hi));
}

static inline uint64_t rdmsr_u64(uint32_t msr)
{
    uint32_t lo, hi;
    __asm__ volatile ("rdmsr" : "=a"(lo), "=d"(hi) : "c"(msr));
    return ((uint64_t)hi << 32) | lo;
}

static inline uint32_t cpuid_ecx(uint32_t leaf)
{
    uint32_t eax, ebx, ecx, edx;
    __asm__ volatile (
        "cpuid"
        : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
        : "a"(leaf), "c"(0)
    );
    (void)ebx;
    (void)edx;
    return ecx;
}

int apic_is_enabled(void)
{
    uint64_t msr = rdmsr_u64(IA32_APIC_BASE_MSR);
    return (msr & APIC_ENABLE) != 0;
}

void apic_init(void)
{
    uint64_t msr = rdmsr_u64(IA32_APIC_BASE_MSR);
    // We do not have IOAPIC routing implemented yet, so enabling APIC here
    // would prevent legacy PIC interrupts (PIT/keyboard) from arriving.
    // Keep the kernel on the PIC path until APIC/IOAPIC support is complete.
    msr &= ~APIC_ENABLE;
    msr &= ~APIC_X2APIC_ENABLE;
    wrmsr_u64(IA32_APIC_BASE_MSR, msr);

    apic_x2apic_enabled = 0;
}

void apic_eoi(void)
{
    if (apic_x2apic_enabled) {
        wrmsr_u64(APIC_EOI_MSR, 0);
    }
}