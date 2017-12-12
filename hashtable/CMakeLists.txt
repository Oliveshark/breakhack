cmake_minimum_required (VERSION 3.2.0)

SET(CMAKE_COLOR_MAKEFILE ON)

project(hashtable C)

add_definitions("-Wall")

add_library(hashtable hashtable.c)

enable_testing()
add_executable(test_table EXCLUDE_FROM_ALL check_hashtable hashtable)
target_compile_options(test_table PRIVATE -pthread)
target_link_libraries(test_table -lcheck)
add_test(test_table test_table)
