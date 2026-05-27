cmake_minimum_required(VERSION 3.20)

find_program(REDDYOS_C_COMPILER
        NAMES x86_64-reddyos-gcc
        PATHS ${CMAKE_SOURCE_DIR}/toolchain/dist/usr/bin
        PATHS ${CMAKE_SOURCE_DIR}/toolchain/sysroot/usr/include
        PATHS ${CMAKE_SOURCE_DIR}/toolchain/sysroot/usr/lib
        PATHS ${CMAKE_SOURCE_DIR}/toolchain/dist/usr/lib/gcc/x86_64-reddyos/16.1.0
        REQUIRED
)
set(CMAKE_C_COMPILER "${REDDYOS_C_COMPILER}" CACHE FILEPATH "C compiler" FORCE)

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

add_custom_target(system ALL
        DEPENDS init
)