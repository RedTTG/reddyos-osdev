#!/bin/bash
mkdir -p dist

(
set -e
cd dist
# Download the latest Limine binary release, only if not already present.
if [ ! -d "limine-binary" ]; then
    curl -L https://github.com/Limine-Bootloader/Limine/releases/download/v12.2.0/limine-binary.tar.gz | gunzip | tar -x
fi

# Build "limine" as a static host tool. Prefer a linux-gnu compiler and avoid
# musl-targeted compilers in the userspace app toolchain.
HOST_CC=""
for cc in gcc cc clang; do
    if ! command -v "$cc" >/dev/null 2>&1; then
        continue
    fi
    cc_path=$(command -v "$cc")
    triplet=$($cc_path -dumpmachine 2>/dev/null || true)
    if [[ "$triplet" == *musl* ]]; then
        continue
    fi
    if [[ "$triplet" == *linux-gnu* ]]; then
        HOST_CC="$cc_path"
        break
    fi
    if [ -z "$HOST_CC" ]; then
        HOST_CC="$cc_path"
    fi
done

if [ -z "$HOST_CC" ]; then
    echo "No suitable non-musl host C compiler found for building limine." >&2
    exit 1
fi

make -C limine-binary clean
make -C limine-binary CC="$HOST_CC" LDFLAGS="-static"

# Ensure limine is static so it does not depend on a host dynamic loader.
if readelf -l limine-binary/limine | grep -q "Requesting program interpreter"; then
    echo "limine is dynamically linked; expected a static binary." >&2
    exit 1
fi

# Create a directory which will be our ISO root.
mkdir -p iso_root

# Copy the relevant files over.
mkdir -p iso_root/boot
cp -v bin/reddyos iso_root/boot/
cp -v bin/initrd.tar iso_root/boot/
mkdir -p iso_root/boot/limine
cp -v ../limine.conf limine-binary/limine-bios.sys limine-binary/limine-bios-cd.bin \
      limine-binary/limine-uefi-cd.bin iso_root/boot/limine/

# Create the EFI boot tree and copy Limine's EFI executables over.
mkdir -p iso_root/EFI/BOOT
cp -v limine-binary/BOOTX64.EFI iso_root/EFI/BOOT/
cp -v limine-binary/BOOTIA32.EFI iso_root/EFI/BOOT/

# Create the bootable ISO.
xorriso -as mkisofs -R -r -J -b boot/limine/limine-bios-cd.bin \
        -no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus \
        -apm-block-size 2048 --efi-boot boot/limine/limine-uefi-cd.bin \
        -efi-boot-part --efi-boot-image --protective-msdos-label \
        iso_root -o image.iso

# Install Limine stage 1 and 2 for legacy BIOS boot.
./limine-binary/limine bios-install image.iso
)