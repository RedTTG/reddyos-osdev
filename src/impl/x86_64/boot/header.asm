section .multiboot_header
header_start:
    ; magic number
    dd 0xe85250d6 ; multiboot2

    ; architecture
    dd 0 ; protected i386

    ; header length
    dd header_end - header_start

    ; checksum
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

    ; end tag
    dd 0 ; type
    dd 0 ; size
    dd 8 ; total size
header_end: