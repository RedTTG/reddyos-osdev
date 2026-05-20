#!/bin/bash
mkdir -p dist

set -e
(

cd mlibc


meson \
  setup \
  --cross-file=../reddyos-cross.txt \
  --prefix=/usr \
  -Dheaders_only=true \
  dist/headers-build

)