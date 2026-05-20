#!/bin/bash
mkdir -p dist
DIST_DIR=$(realpath dist)
REDDYOS_ROOT=$(realpath .)
CROSS_FILE="$REDDYOS_ROOT/toolchain/reddyos-cross.txt"

set -e
(

cd toolchain/mlibc


meson \
  setup \
  --cross-file="$CROSS_FILE" \
  --prefix=/usr \
  -Dheaders_only=true \
  "$DIST_DIR/headers-build"

)