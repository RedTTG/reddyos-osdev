#!/bin/bash
set -e

# Build the kernel and the bootloader.
mkdir -p build
(
cd build
cmake ..
)
mkdir -p dist
cmake --build build

# Build the ISO image.
./make_iso.sh

# Run the ISO image in QEMU.
qemu-system-x86_64 -s -S -machine q35 -cdrom dist/image.iso -m 512M -serial stdio -no-reboot -no-shutdown
#-M accel=tcg,smm=off -d int