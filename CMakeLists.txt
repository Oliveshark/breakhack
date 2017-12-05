cmake_minimum_required (VERSION 3.2.0)

SET(CMAKE_COLOR_MAKEFILE ON)

project(breakhack C)

include(FindLua)

add_subdirectory(linkedlist)

include_directories(linkedlist
	${LUA_INCLUDE_DIRS}
	)

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
	src/map_lua
	src/camera
	src/timer
	)

target_link_libraries(breakhack
	linkedlist
	${LUA_LIBRARIES}
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
