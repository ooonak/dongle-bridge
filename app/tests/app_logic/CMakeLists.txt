# tests/unit/CMakeLists.txt

cmake_minimum_required(VERSION 3.20.0)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(app_logic_tests)

include(../../cmake/flags.cmake)

# Test sources
target_sources(app PRIVATE
    src/test_app_logic.c
    src/fake_port.c

    # Pull in the real app logic
    ${CMAKE_CURRENT_LIST_DIR}/../../src/app/app_logic.c
)

# Include project headers
target_include_directories(app PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../include
)
