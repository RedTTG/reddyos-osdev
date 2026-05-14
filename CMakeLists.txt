cmake_minimum_required(VERSION 3.20)

project(reddyos C ASM_NASM)

set(CMAKE_C_STANDARD 11)

enable_language(ASM_NASM)

# Prefer clang if no compiler specified, but allow user override via cache
if(NOT DEFINED CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER clang CACHE STRING "C compiler")
endif()

# Find nasm and record it for ASM_NASM builds
find_program(NASM_EXECUTABLE nasm)
if(NOT NASM_EXECUTABLE)
    message(FATAL_ERROR "nasm not found in PATH; please install nasm or set CMAKE_ASM_NASM_COMPILER")
endif()
set(CMAKE_ASM_NASM_COMPILER "${NASM_EXECUTABLE}" CACHE STRING "NASM assembler")

# C Compiler flags (applied to the target below)
set(_REDDYOS_C_FLAGS
    -ffreestanding
    -fno-stack-protector
    -fno-stack-check
    -fno-lto
    -fno-PIC
    -ffunction-sections
    -fdata-sections
    -m64
    -march=x86-64
    -mabi=sysv
    -mno-80387
    -mno-mmx
    -mno-sse
    -mno-sse2
    -mno-red-zone
    -mcmodel=kernel
    -Wall
    -Wextra
    -g
    -O2
)

# Add clang-only -target flags using a generator expression so the flags are
# applied only when the actual compiler is Clang at build time. This avoids
# passing '-target' to GCC/cc (which will error) even if CMake's configure-time
# detection was inconsistent with the compiler used at build time.
list(PREPEND _REDDYOS_C_FLAGS
    $<$<C_COMPILER_ID:Clang>:-target>
    $<$<C_COMPILER_ID:Clang>:x86_64-unknown-none-elf>
)

# Linker flags
set(CMAKE_EXE_LINKER_FLAGS_INIT "
    -nostdlib
    -static
    -m elf_x86_64
    -z max-page-size=0x1000
    --gc-sections
    -T ${CMAKE_SOURCE_DIR}/linker.lds
")

# Include directories
include_directories(
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/headers
)

# Also add each subdirectory under headers/ so files that do `#include "common.h"`
# (located in headers/kernel_common/common.h) can be found without changing source.
file(GLOB_RECURSE _HEADER_FILES "${CMAKE_SOURCE_DIR}/headers/*.h")
set(_HEADER_DIRS)
foreach(_hf IN LISTS _HEADER_FILES)
    get_filename_component(_dir ${_hf} PATH)
    list(APPEND _HEADER_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES _HEADER_DIRS)
foreach(_d IN LISTS _HEADER_DIRS)
    include_directories(${_d})
endforeach()

# Recursively find all source files
file(GLOB_RECURSE C_SOURCES
        ${CMAKE_SOURCE_DIR}/src/*.c
)

file(GLOB_RECURSE ASM_NASM_SOURCES
        ${CMAKE_SOURCE_DIR}/src/*.asm
)

file(GLOB_RECURSE ASM_GAS_SOURCES
        ${CMAKE_SOURCE_DIR}/src/*.S
)

# Configure NASM compiler
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
set(CMAKE_ASM_NASM_FLAGS "-f elf64 -g -F dwarf -Wall")

# Mark NASM files with appropriate language
set_source_files_properties(${ASM_NASM_SOURCES}
        PROPERTIES
        LANGUAGE ASM_NASM
)

# Mark GAS (.S) files as assembler
set_source_files_properties(${ASM_GAS_SOURCES}
        PROPERTIES
        LANGUAGE ASM
)

# Create the executable
add_executable(reddyos
        ${C_SOURCES}
        ${ASM_NASM_SOURCES}
        ${ASM_GAS_SOURCES}
)

# Apply compilation flags to the target explicitly but only for C sources so
# they are not passed to NASM. Use generator expressions to scope per-language.
foreach(_flag IN LISTS _REDDYOS_C_FLAGS)
    target_compile_options(reddyos PRIVATE $<$<COMPILE_LANGUAGE:C>:${_flag}>)
endforeach()

# Ensure the linker is invoked in freestanding mode and does not pull in the
# host C runtime (Scrt1.o / crt objects). We also pass the custom linker
# script and common kernel link-time options.
target_link_options(reddyos PRIVATE
    -nostdlib
    -nostartfiles
    -nodefaultlibs
    -static
    -Wl,-m,elf_x86_64
    -Wl,-z,max-page-size=0x1000
    -Wl,--gc-sections
    -T${CMAKE_SOURCE_DIR}/linker.lds
)

# Set output name and suffix
set_target_properties(reddyos PROPERTIES
        SUFFIX ""
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/dist/bin"
)

# Build the Limine tarfs initrd from files/ into dist/root/ and package it as
# dist/bin/initrd.tar for the ISO build step.
file(GLOB_RECURSE INITRD_ROOT_FILES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/files/*"
)

set(INITRD_ROOT_DIR "${CMAKE_SOURCE_DIR}/dist/root")
set(INITRD_TAR "${CMAKE_SOURCE_DIR}/dist/bin/initrd.tar")

add_custom_command(
        OUTPUT "${INITRD_TAR}"
        COMMAND "${CMAKE_COMMAND}" -E rm -rf "${INITRD_ROOT_DIR}"
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${INITRD_ROOT_DIR}"
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_SOURCE_DIR}/files" "${INITRD_ROOT_DIR}"
        # Ensure bin dir exists inside the root and copy the built init executable into it.
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${INITRD_ROOT_DIR}/bin"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${CMAKE_SOURCE_DIR}/dist/bin/system/init" "${INITRD_ROOT_DIR}/bin/init"
        COMMAND tar -cf "${INITRD_TAR}" -C "${INITRD_ROOT_DIR}" .
        DEPENDS ${INITRD_ROOT_FILES} "${CMAKE_SOURCE_DIR}/dist/bin/system/init"
        VERBATIM
)

add_custom_target(initrd_tar ALL DEPENDS "${INITRD_TAR}")

# Build the small userspace `init` program and make sure it's available under
# dist/bin so the initrd packaging step above can copy it into dist/root/bin.
add_subdirectory(system/init)

# Ensure the packaging target depends on the `init` target so the init ELF is
# built before the TAR is created.
add_dependencies(initrd_tar init)

# Add linker script as a dependency
set_target_properties(reddyos PROPERTIES
        LINK_DEPENDS "${CMAKE_SOURCE_DIR}/linker.lds"
)