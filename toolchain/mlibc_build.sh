#!/bin/bash
mkdir -p dist
REDDYOS_ROOT=$(realpath ..)
TOOLCHAIN_DIR=$(realpath .)
DIST_DIR="$TOOLCHAIN_DIR/dist"
SYSROOT_DIR="$TOOLCHAIN_DIR/sysroot"
CROSS_FILE="$TOOLCHAIN_DIR/reddyos-cross.txt"
set -e
(
export PATH="${DIST_DIR}/usr/bin:${PATH}"

cd mlibc

meson \
  setup \
  --cross-file="$CROSS_FILE" \
  --prefix=/usr \
  -Ddefault_library=static \
  -Dno_headers=true \
  build

DESTDIR=${SYSROOT_DIR} ninja -C build install

)