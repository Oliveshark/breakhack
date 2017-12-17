cmake_minimum_required (VERSION 3.2.0)

SET(CMAKE_COLOR_MAKEFILE ON)

project(breakhack C)

include(FindLua)
include(FindX11)
include(cmake/FindSDL2.cmake)
include(cmake/FindSDL2_image.cmake)
include(cmake/FindSDL2_mixer.cmake)
include(cmake/FindSDL2_ttf.cmake)
include(cmake/FindCheck.cmake)

add_subdirectory(linkedlist)
add_subdirectory(hashtable)

include_directories(
	linkedlist
	hashtable
	${LUA_INCLUDE_DIR}
	${SDL2_INCLUDE_DIR}
	${SDL2_IMAGE_INCLUDE_DIR}
	${SDL2_MIXER_INCLUDE_DIR}
	${SDL2_TTF_INCLUDE_DIR}
	${X11_INCLUDE_DIR}
	${CHECK_INCLUDE_DIR}
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
	src/monster
	src/stats
	src/actiontext
	src/random
	)

target_link_libraries(breakhack
	linkedlist
	hashtable
	${LUA_LIBRARY}
	${SDL2_LIBRARY}
	${SDL2_IMAGE_LIBRARY}
	${SDL2_MIXER_LIBRARY}
	${SDL2_TTF_LIBRARY}
	${X11_LIBRARIES}
	)

# TESTS:
IF (CHECK_FOUND)
	find_package(Threads REQUIRED)
	enable_testing()
	add_executable(test_util test/check_util src/util)
	target_link_libraries(test_util ${CHECK_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
	add_test(test_util test_util)
ENDIF (CHECK_FOUND)
