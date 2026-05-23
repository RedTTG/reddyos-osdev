
TOOLCHAIN_DIR=$(realpath .)
SYSROOT_DIR="${TOOLCHAIN_DIR}/sysroot"
DEST_DIR="${TOOLCHAIN_DIR}/dist"
PREFIX="/usr"

(
set -e

export PATH="${DEST_DIR}/usr/bin:$PATH"
cd binutils
mkdir -p build
cd build

# --with-sysroot will tell the linker where to search for libraries.
# --enable-default-execstack=no will tell the linker to
#   not use an executable stack by default.
../configure \
    --target=x86_64-reddyos \
    --prefix="${PREFIX}" \
    --with-sysroot="${SYSROOT_DIR}" \
    --disable-werror \
    --enable-default-execstack=no

sudo make -j$(nproc)

DESTDIR="${DEST_DIR}" make install
)