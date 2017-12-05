cmake_minimum_required (VERSION 3.2.0)

SET(CMAKE_COLOR_MAKEFILE ON)

project(linkedlist C)

add_definitions("-Wall")

add_library(linkedlist linkedlist.c)

enable_testing()
add_executable(test_list check_linkedlist linkedlist)
target_compile_options(test_list PRIVATE -pthread)
target_link_libraries(test_list -lcheck)
add_test(test_list test_list)
