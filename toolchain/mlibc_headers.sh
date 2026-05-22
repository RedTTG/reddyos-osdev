#!/bin/bash
mkdir -p dist
REDDYOS_ROOT=$(realpath ..)
TOOLCHAIN_DIR=$(realpath .)
DIST_DIR="$TOOLCHAIN_DIR/dist"
MLIBC_DIR="$TOOLCHAIN_DIR/mlibc"
SYSROOT_DIR="$TOOLCHAIN_DIR/sysroot"
CROSS_FILE="$TOOLCHAIN_DIR/reddyos-cross.txt"
HEADERS_BUILD_DIR="${MLIBC_DIR}/headers-build"
set -e
(

cd mlibc


meson \
  setup \
  --cross-file="$CROSS_FILE" \
  --prefix=/usr \
  -Dheaders_only=true \
  "$HEADERS_BUILD_DIR"
)

DESTDIR=${SYSROOT_DIR} ninja -C "$HEADERS_BUILD_DIR" install