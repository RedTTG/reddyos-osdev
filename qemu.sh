#!/usr/bin/env bash
set -e

make build_x86_64
qemu-system-x86_64 -cdrom dist/x86_64/os.iso
