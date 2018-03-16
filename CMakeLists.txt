cmake_minimum_required (VERSION 3.2.0)

SET(CMAKE_COLOR_MAKEFILE ON)

project(breakhack C)

set(breakhack_GAME_TITLE "BreakHack")
set(breakhack_MAJOR_VERSION 0)
set(breakhack_MINOR_VERSION 1)
set(breakhack_PATCH_VERSION 1)
set(breakhack_RELEASE_TYPE "(early access)")

include(FindLua)
include(FindPhysFS)
include(cmake/FindSDL2.cmake)
include(cmake/FindSDL2_image.cmake)
include(cmake/FindSDL2_ttf.cmake)
include(cmake/FindSDL2_mixer.cmake)
include(cmake/FindCCache.cmake)
include(cmake/Findcppcheck.cmake)

if (NOT WIN32)
	include(FindX11)
	include(cmake/FindCheck.cmake)
endif (NOT WIN32)

configure_file(
	"${PROJECT_SOURCE_DIR}/src/config.h.in"
	"${PROJECT_BINARY_DIR}/config.h"
	)

IF ( WIN32 )
    MESSAGE ( STATUS "Setting MSVC MT switches")
    SET (
      CMAKE_C_FLAGS_DEBUG
        "${CMAKE_CXX_FLAGS_DEBUG} /MTd"
        CACHE STRING "MSVC MT flags " FORCE
    )
    SET (
      CMAKE_C_FLAGS_RELEASE
        "${CMAKE_CXX_FLAGS_RELEASE} /MT"
        CACHE STRING "MSVC MT flags " FORCE
    )
  ENDIF ( WIN32 )

if (NOT PHYSFS_FOUND)
	add_subdirectory(physfs-3.0.1)
	include_directories(
		physfs-3.0.1/src
		)
else (NOT PHYSFS_FOUND)
	include_directories(
		${PHYSFS_INCLUDE_DIR}
		)
endif (NOT PHYSFS_FOUND)

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
	src/io_util
	src/physfsrwops
	src/skillbar
	src/texturecache
	src/skill
	src/projectile
	src/vector2d
	src/map_room_modifiers
	src/sqlite3
	)

# Sqlite has some warnings that I we don't need to see
set_source_files_properties(src/sqlite3.c COMPILE_FLAGS -w)

target_link_libraries(breakhack
	${CMAKE_DL_LIBS} # Sqlite needs DL libs
	${LUA_LIBRARY}
	${SDL2_LIBRARY}
	${SDL2MAIN_LIBRARY}
	${SDL2_IMAGE_LIBRARY}
	${SDL2_TTF_LIBRARY}
	${SDL2_MIXER_LIBRARY}
	)

if (NOT PHYSFS_FOUND)
	target_link_libraries(breakhack
		physfs
		)
else (NOT PHYSFS_FOUND)
	target_link_libraries(breakhack
		${PHYSFS_LIBRARY}
		)
endif (NOT PHYSFS_FOUND)

if (NOT WIN32)
	target_link_libraries(breakhack
		${X11_LIBRARIES}
		)
endif (NOT WIN32)

if (MSVC)
	set_target_properties(breakhack PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
	set_target_properties(breakhack PROPERTIES COMPILE_DEFINITIONS_DEBUG "_CONSOLE")
	set_target_properties(breakhack PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:CONSOLE")
	set_target_properties(breakhack PROPERTIES COMPILE_DEFINITIONS_RELWITHDEBINFO "_CONSOLE")
	set_target_properties(breakhack PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
	set_target_properties(breakhack PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS")
endif (MSVC)

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

# LINT:
if (CPPCHECK_FOUND)
	add_custom_target(lint
		COMMAND ${CPPCHECK_EXECUTABLE} --language=c --template=gcc --error-exitcode=1 --quiet --enable=warning,style,performance,portability,information,missingInclude src/
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
		COMMENT "Run cppcheck"
		)
endif (CPPCHECK_FOUND)

if (NOT CMAKE_BUILD_TYPE MATCHES Debug)
	add_custom_target(assets_pack ALL
		COMMAND ${CMAKE_COMMAND} -E tar "-cf" "../assets.pack" "--format=zip" --
			"${CMAKE_CURRENT_SOURCE_DIR}/assets/Characters"
			"${CMAKE_CURRENT_SOURCE_DIR}/assets/Commissions"
			"${CMAKE_CURRENT_SOURCE_DIR}/assets/GUI"
			"${CMAKE_CURRENT_SOURCE_DIR}/assets/Items"
			"${CMAKE_CURRENT_SOURCE_DIR}/assets/Objects"
			"${CMAKE_CURRENT_SOURCE_DIR}/assets/Sounds"
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets
		)

	add_custom_target(data_pack ALL
		COMMAND ${CMAKE_COMMAND} -E tar "-cf" "../data.pack" "--format=zip" --
			"mapgen.lua"
			"maproombuilder.lua"
			"menumapgen.lua"
			"monstergen.lua"
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data
		)
endif (NOT CMAKE_BUILD_TYPE MATCHES Debug)

SET(CMAKE_INSTALL_SYSTEM_RUNTIME_COMPONENT "Release")
SET(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ".")
if (WIN32)
	SET(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS
		${CMAKE_SOURCE_DIR}/bin/libFLAC-8.dll
		${CMAKE_SOURCE_DIR}/bin/libfreetype-6.dll
		${CMAKE_SOURCE_DIR}/bin/libmodplug-1.dll
		${CMAKE_SOURCE_DIR}/bin/libmpg123-0.dll
		${CMAKE_SOURCE_DIR}/bin/libogg-0.dll
		${CMAKE_SOURCE_DIR}/bin/libpng16-16.dll
		${CMAKE_SOURCE_DIR}/bin/libvorbis-0.dll
		${CMAKE_SOURCE_DIR}/bin/libvorbisfile-3.dll
		${CMAKE_SOURCE_DIR}/bin/physfs.dll
		${CMAKE_SOURCE_DIR}/bin/SDL2.dll
		${CMAKE_SOURCE_DIR}/bin/SDL2_image.dll
		${CMAKE_SOURCE_DIR}/bin/SDL2_mixer.dll
		${CMAKE_SOURCE_DIR}/bin/SDL2_ttf.dll
		${CMAKE_SOURCE_DIR}/bin/zlib1.dll
		)
endif (WIN32)
include(InstallRequiredSystemLibraries)

INSTALL(TARGETS breakhack
	COMPONENT Release
	RUNTIME DESTINATION .
)
INSTALL(FILES assets.pack data.pack
	COMPONENT Release
	DESTINATION .
)

set(CPACK_INSTALL_CMAKE_PROJECTS "${PROJECT_BINARY_DIR};breakhack;Release;.")
set(CPACK_PACKAGE_NAME "BreakHack")
set(CPACK_PACKAGE_VENDOR "OliveShark")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_PACKAGE_DIRECTORY ${PROJECT_BINARY_DIR}/package)
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_VERSION_PATCH "1")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "BreakHack")
set(CPACK_PACKAGE_FILE_NAME "breakhack-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_PACKAGE_CHECKSUM "MD5")

if(UNIX)
	set(CPACK_STRIP_FILES breakhack)
	set(CPACK_SOURCE_STRIP_FILES "")
endif(UNIX)

include(CPack)
