cmake_minimum_required(VERSION 3.20)

project(systest C)

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