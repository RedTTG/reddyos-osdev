cmake_minimum_required(VERSION 3.20)

# Use the musl userspace compiler for system/ targets by default. Setting
# CMAKE_C_COMPILER before calling project() makes this directory use the
# musl toolchain while the top-level kernel build can keep using clang.
find_program(MUSL_C_COMPILER
        NAMES x86_64-unknown-linux-musl-gcc musl-gcc
        REQUIRED
)
set(CMAKE_C_COMPILER "${MUSL_C_COMPILER}" CACHE FILEPATH "C compiler" FORCE)

project(systest C)

# systest is a freestanding test binary (oddity). Keep its freestanding
# compile and link flags so it behaves like a kernel-style program.
add_executable(systest
        systest/src/main.c
)

target_include_directories(systest BEFORE PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/systest/headers"
)

set_target_properties(systest PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/dist/bin/system"
        SUFFIX ""
)

target_compile_options(systest PRIVATE
        -ffreestanding
        -fno-stack-protector
        -fno-pie
        -no-pie
        -nostdlib
        -O2
        -g
)

target_link_options(systest PRIVATE
        -nostdlib
        -static
        -no-pie
        -Wl,-e,_start
)

# init is a normal hosted userspace program built with musl. Do not use
# freestanding or -nostdlib flags here; musl will provide the C runtime.
add_executable(init
        init/src/main.c
)

set_target_properties(init PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/dist/bin/system"
        SUFFIX ""
)

target_compile_options(init PRIVATE
        -O2
        -g
)

target_link_options(init PRIVATE
        -static
)

