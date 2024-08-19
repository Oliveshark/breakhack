cmake_minimum_required (VERSION 3.6)

SET(CMAKE_COLOR_MAKEFILE ON)

project(lua C)

set(lua_MAJOR_VERSION 5)
set(lua_MINOR_VERSION 3)
set(lua_PATCH_VERSION 5)

if (NOT MSVC AND NOT MINGW)
	add_definitions(-DLUA_USE_POSIX)
endif ()

add_library(lua STATIC
	src/lapi.c
	src/lauxlib.c
	src/lbaselib.c
	src/lbitlib.c
	src/lcode.c
	src/lcorolib.c
	src/lctype.c
	src/ldblib.c
	src/ldebug.c
	src/ldo.c
	src/ldump.c
	src/lfunc.c
	src/lgc.c
	src/linit.c
	src/liolib.c
	src/llex.c
	src/lmathlib.c
	src/lmem.c
	src/loadlib.c
	src/lobject.c
	src/lopcodes.c
	src/loslib.c
	src/lparser.c
	src/lstate.c
	src/lstring.c
	src/lstrlib.c
	src/ltable.c
	src/ltablib.c
	src/ltm.c
	src/lundump.c
	src/lutf8lib.c
	src/lvm.c
	src/lzio.c
	)

if (NOT MSVC)
	target_link_libraries(lua
		-lm
		)
endif ()
