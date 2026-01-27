cmake_minimum_required(VERSION 3.13.1)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

zephyr_include_directories(${CMAKE_CURRENT_LIST_DIR}/../../include)

target_sources(app PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/test_sum.c
    ${CMAKE_CURRENT_LIST_DIR}/../../src/sum.c
)
