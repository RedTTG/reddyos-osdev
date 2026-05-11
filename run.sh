#!/bin/bash
set -e

# Build the kernel and the bootloader.
make

# Build the ISO image.
./make_iso.sh

# Run the ISO image in QEMU.
qemu-system-x86_64 -machine q35 -cdrom dist/image.iso -m 512M -serial stdio
