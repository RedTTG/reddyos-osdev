
TOOLCHAIN_DIR=$(realpath .)
SYSROOT_DIR="${TOOLCHAIN_DIR}/sysroot"
DEST_DIR="${TOOLCHAIN_DIR}/dist"
PREFIX="/usr"

(
set -e

export PATH="${DEST_DIR}/usr/bin:${PATH}"
cd gcc
mkdir -p build
cd build

# --with-sysroot will tell the linker where to search for libraries.
# --enable-default-execstack=no will tell the linker to
#   not use an executable stack by default.
../configure \
    --target=x86_64-reddyos \
    --prefix="${PREFIX}" \
    --with-sysroot="${SYSROOT_DIR}" \
    --enable-languages=c,c++ \
    --enable-threads=posix \
    --disable-multilib \
    --enable-shared \
    --enable-host-shared

make -j$(nproc) all-gcc all-target-libgcc

DESTDIR="${DEST_DIR}" make install-gcc install-target-libgcc
)