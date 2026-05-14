#pragma once

#define IA32_EFER   0xC0000080
#define IA32_STAR   0xC0000081
#define IA32_LSTAR  0xC0000082
#define IA32_FMASK  0xC0000084

#define EFER_SCE (1 << 0)

extern void syscall_entry(void);
extern void syscall_handler(void);
void syscall_init(void);