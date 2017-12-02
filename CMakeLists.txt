cmake_minimum_required (VERSION 3.2.0)

SET(CMAKE_COLOR_MAKEFILE ON)

project(breakhack C)

add_subdirectory(linkedlist)
include_directories(linkedlist)
add_definitions("-Wall")

# PROGRAMS:
add_executable(breakhack
	src/main
	src/texture
	src/screenresolution
	src/sprite
	src/util
	src/player
	src/map
	src/camera
	)

target_link_libraries(breakhack
	linkedlist
	-lSDL2
	-lSDL2_image
	-lSDL2_mixer
	-lSDL2_ttf
	-lX11
	-lXrandr
	)

# TESTS:
enable_testing()

add_executable(test_util test/check_util src/util)
target_link_libraries(test_util -lcheck)
add_test(test_util test_util)
