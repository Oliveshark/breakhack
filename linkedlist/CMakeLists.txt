cmake_minimum_required (VERSION 3.2.0)

SET(CMAKE_COLOR_MAKEFILE ON)

include("../cmake/FindCheck.cmake")

project(linkedlist C)

add_definitions("-Wall")

add_library(linkedlist linkedlist.c)

include_directories(${CHECK_INCLUDE_DIR})

if (CHECK_FOUND)
	find_package(Threads REQUIRED)
	enable_testing()
	add_executable(test_list check_linkedlist linkedlist)
	target_compile_options(test_list PRIVATE -pthread)
	target_link_libraries(test_list ${CHECK_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
	add_test(test_list test_list)
endif (CHECK_FOUND)
