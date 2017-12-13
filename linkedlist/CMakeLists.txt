cmake_minimum_required (VERSION 3.2.0)

SET(CMAKE_COLOR_MAKEFILE ON)

include("../cmake/FindCheck.cmake")

project(linkedlist C)

add_definitions("-Wall")

add_library(linkedlist linkedlist.c)

include_directories(${CHECK_INCLUDE_DIR})

if (CHECK_FOUND)
	enable_testing()
	add_executable(test_list check_linkedlist linkedlist)
	target_compile_options(test_list PRIVATE -pthread)
	target_link_libraries(test_list ${CHECK_LIBRARIES})
	add_test(test_list test_list)
endif (CHECK_FOUND)
