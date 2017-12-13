cmake_minimum_required (VERSION 3.2.0)

SET(CMAKE_COLOR_MAKEFILE ON)

include("../cmake/FindCheck.cmake")

project(hashtable C)

add_definitions("-Wall")

add_library(hashtable hashtable.c)

include_directories(${CHECK_INCLUDE_DIR})

if (CHECK_FOUND)
	enable_testing()
	add_executable(test_table check_hashtable hashtable)
	target_compile_options(test_table PRIVATE -pthread)
	target_link_libraries(test_table ${CHECK_LIBRARIES})
	add_test(test_table test_table)
endif (CHECK_FOUND)
