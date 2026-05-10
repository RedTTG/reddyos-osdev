#!/bin/bash

# Build the kernel and the bootloader.
make

# Build the ISO image.
./make_iso.sh

# Run the ISO image in QEMU.
qemu-system-x86_64 -cdrom dist/image.iso -m 512M