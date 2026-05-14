cmake_minimum_required(VERSION 3.20)

project(init C)

add_executable(init
    src/main.c
)

set_target_properties(init PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/dist/bin/system"
    SUFFIX ""
)

target_compile_options(init PRIVATE -g -O2)


