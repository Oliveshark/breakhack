cmake_minimum_required (VERSION 3.2.0)

SET(CMAKE_COLOR_MAKEFILE ON)

project(breakhack C)

include(FindLua)

add_subdirectory(linkedlist)
add_subdirectory(hashtable)

include_directories(linkedlist
	${LUA_INCLUDE_DIR}
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
	src/roommatrix
	src/position
	)

target_link_libraries(breakhack
	linkedlist
	hashtable
	${LUA_LIBRARIES}
	-lSDL2
	-lSDL2_image
	-lSDL2_mixer
	-lSDL2_ttf
	-lX11
	)

# TESTS:
enable_testing()

add_executable(test_util EXCLUDE_FROM_ALL test/check_util src/util)
target_compile_options(test_util PRIVATE -pthread)
target_link_libraries(test_util -lcheck)
add_test(test_util test_util)
