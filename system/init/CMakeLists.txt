cmake_minimum_required(VERSION 3.20)

project(init C)

add_executable(init
        src/main.c
)

target_include_directories(init BEFORE PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/headers"
)

set_target_properties(init PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/dist/bin/system"
        SUFFIX ""
)

target_compile_options(init PRIVATE
        -ffreestanding
        -fno-stack-protector
        -fno-pie
        -no-pie
        -nostdlib
        -O2
        -g
)

target_link_options(init PRIVATE
        -nostdlib
        -static
        -no-pie
        -Wl,-e,_start
)