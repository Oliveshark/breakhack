cmake_minimum_required (VERSION 3.2.0)

SET(CMAKE_COLOR_MAKEFILE ON)

project(breakhack C)

include(FindLua)
include(cmake/FindSDL2.cmake)
include(cmake/FindSDL2_image.cmake)
include(cmake/FindSDL2_ttf.cmake)
include(cmake/FindSDL2_mixer.cmake)
include(cmake/FindCCache.cmake)

if (NOT WIN32)
	include(FindX11)
	include(cmake/FindCheck.cmake)
endif (NOT WIN32)

configure_file(
	"${PROJECT_SOURCE_DIR}/src/config.h.in"
	"${PROJECT_BINARY_DIR}/config.h"
	)

include_directories(
	${PROJECT_BINARY_DIR}
	${LUA_INCLUDE_DIR}
	${SDL2_INCLUDE_DIR}
	${SDL2_IMAGE_INCLUDE_DIR}
	${SDL2_TTF_INCLUDE_DIR}
	${SDL2_MIXER_INCLUDE_DIR}
	)

if (NOT WIN32)
	include_directories(
		${X11_INCLUDE_DIR}
		${CHECK_INCLUDE_DIR}
		)
endif (NOT WIN32)

if (NOT WIN32)
	add_definitions(-std=gnu11
		-pedantic -Wall -Wextra -Wshadow
		-Wpointer-arith -Wcast-qual
		-Wstrict-prototypes
		-Wmissing-prototypes
		-Wconversion -Wno-sign-conversion
		)
endif (NOT WIN32)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")

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
	src/linkedlist
	src/hashtable
	src/gui
	src/item
	src/item_builder
	src/pointer
	src/gui_button
	src/particle_engine
	src/menu
	src/collisions
	src/keyboard
	src/mixer
	)

target_link_libraries(breakhack
	${LUA_LIBRARY}
	${SDL2_LIBRARY}
	${SDL2_IMAGE_LIBRARY}
	${SDL2_TTF_LIBRARY}
	${SDL2_MIXER_LIBRARY}
	)

if (NOT WIN32)
	target_link_libraries(breakhack
		${X11_LIBRARIES}
		)
endif (NOT WIN32)

# TESTS:
IF (CHECK_FOUND AND NOT WIN32)
	find_package(Threads REQUIRED)
	enable_testing()
	add_executable(test_util test/check_util src/util)
	target_link_libraries(test_util ${CHECK_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
	add_test(test_util test_util)

	add_executable(test_linkedlist test/check_linkedlist src/linkedlist src/util)
	target_link_libraries(test_linkedlist ${CHECK_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
	add_test(test_linkedlist test_linkedlist)

	add_executable(test_hashtable test/check_hashtable src/hashtable src/util)
	target_link_libraries(test_hashtable ${CHECK_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
	add_test(test_hashtable test_hashtable)
ENDIF (CHECK_FOUND AND NOT WIN32)
