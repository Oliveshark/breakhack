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
include(build/cmake/FindSDL2.cmake)
include(build/cmake/FindSDL2_image.cmake)
include(build/cmake/FindSDL2_ttf.cmake)
include(build/cmake/FindSDL2_mixer.cmake)
include(build/cmake/FindCCache.cmake)
include(build/cmake/FindCMocka.cmake)
include(build/cmake/Findcppcheck.cmake)

configure_file(
	"${PROJECT_SOURCE_DIR}/src/config.h.in"
	"${PROJECT_BINARY_DIR}/config.h"
	)

if ("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
	set(CLANG 1)
elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
	set(GCC 1)
endif()

set (default_build_type "Release")
if (NOT CMAKE_BUILD_TYPE AND default_build_type)
	message( STATUS "Setting default buildtype to '${default_build_type}' as none was specified")
	set(CMAKE_BUILD_TYPE "${default_build_type}")
endif ()

# Deal with travis compile issue
if (CCACHE_FOUND AND CLANG)
	add_definitions(-Qunused-arguments -fcolor-diagnostics)
endif()

if (CMAKE_BUILD_TYPE MATCHES "Debug")
	set(DEBUG_BUILD TRUE)
endif()

IF ( MSVC )
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
ELSEIF ( WIN32 )
    SET (
      CMAKE_C_FLAGS_DEBUG
        "${CMAKE_CXX_FLAGS_DEBUG} -mconsole"
    )
    SET (
      CMAKE_C_FLAGS_RELEASE
        "${CMAKE_CXX_FLAGS_RELEASE} -mwindows"
    )
ENDIF ()
IF ( GCC OR CLANG )
    SET (
      CMAKE_C_FLAGS_DEBUG
        "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer"
		)
ENDIF ( GCC OR CLANG )

if (NOT PHYSFS_FOUND OR WIN32)
	set(PHYSFS_BUILD_STATIC TRUE)
	set(PHYSFS_BUILD_SHARED FALSE)
	set(PHYSFS_BUILD_TEST FALSE)
	add_subdirectory(physfs-3.0.1)
	include_directories(
		physfs-3.0.1/src
		)
else ()
	include_directories(
		${PHYSFS_INCLUDE_DIR}
		)
endif ()

include_directories(
	${PROJECT_BINARY_DIR}
	${LUA_INCLUDE_DIR}
	${SDL2_INCLUDE_DIR}
	${SDL2_IMAGE_INCLUDE_DIR}
	${SDL2_TTF_INCLUDE_DIR}
	${SDL2_MIXER_INCLUDE_DIR}
	)

if (CMOCKA_FOUND)
	include_directories(
		${CMOCKA_INCLUDE_DIR}
		)
endif(CMOCKA_FOUND)

if (NOT MSVC)
	add_definitions(-std=gnu11
		-pedantic -Wall -Wextra -Wshadow
		-Wpointer-arith -Wcast-qual
		-Wstrict-prototypes
		-Wmissing-prototypes
		-Wconversion -Wno-sign-conversion
		)
	if (NOT CLANG)
		add_definitions(-static -static-libgcc -static-libstdc++)
	endif ()
endif (NOT MSVC)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")
if (NOT MSVC)
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D__FNAME__='\"$(subst ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")
else (NOT MSVC)
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D__FNAME__=__FILE__")
endif (NOT MSVC)

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
	src/db
	src/settings
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
		physfs-static
		)
else ()
	target_link_libraries(breakhack
		${PHYSFS_LIBRARY}
		)
endif ()

if (MSVC)
	set_target_properties(breakhack PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
	set_target_properties(breakhack PROPERTIES COMPILE_DEFINITIONS_DEBUG "_CONSOLE")
	set_target_properties(breakhack PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:CONSOLE")
	set_target_properties(breakhack PROPERTIES COMPILE_DEFINITIONS_RELWITHDEBINFO "_CONSOLE")
	set_target_properties(breakhack PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
	set_target_properties(breakhack PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS")
endif (MSVC)

# TESTS:
IF (CMOCKA_FOUND)
	find_package(Threads REQUIRED)
	enable_testing()
	add_executable(test_util test/test_util src/util)
	target_link_libraries(test_util ${CMOCKA_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
	add_test(test_util test_util)

	add_executable(test_linkedlist test/test_linkedlist src/linkedlist src/util)
	target_link_libraries(test_linkedlist ${CMOCKA_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
	add_test(test_linkedlist test_linkedlist)

	add_executable(test_hashtable test/test_hashtable src/hashtable src/util)
	target_link_libraries(test_hashtable ${CMOCKA_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
	add_test(test_hashtable test_hashtable)
ENDIF (CMOCKA_FOUND )

# LINT:
if (CPPCHECK_FOUND)
	add_custom_target(lint
		COMMAND ${CPPCHECK_EXECUTABLE} --force --language=c --template=gcc --error-exitcode=1 --quiet --enable=warning,style,performance,portability,information,missingInclude --suppress=*:src/sqlite3.? -isrc/sqlite3.c src/
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
		COMMENT "Run cppcheck"
		)
endif (CPPCHECK_FOUND)

if (NOT DEBUG_BUILD)
	add_custom_target(assets_pack ALL
		COMMAND ${CMAKE_COMMAND} -E tar "-cf" "../assets.pack" "--format=zip" --
			"${CMAKE_CURRENT_SOURCE_DIR}/assets/Characters"
			"${CMAKE_CURRENT_SOURCE_DIR}/assets/Commissions"
			"${CMAKE_CURRENT_SOURCE_DIR}/assets/GUI"
			"${CMAKE_CURRENT_SOURCE_DIR}/assets/Items"
			"${CMAKE_CURRENT_SOURCE_DIR}/assets/Objects"
			"${CMAKE_CURRENT_SOURCE_DIR}/assets/Sounds"
			"${CMAKE_CURRENT_SOURCE_DIR}/assets/Extras"
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
	if (WIN32)
		add_definitions(-mwindows)
	endif()
else()
	if (WIN32)
		add_definitions(-mconsole)
	endif()
endif ()

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
	set(CPACK_GENERATOR STGZ TGZ TZ)
	set(CPACK_STRIP_FILES breakhack)
	set(CPACK_SOURCE_STRIP_FILES "")
elseif(WIN32)
	set(CPACK_GENERATOR NSIS ZIP)
endif()

include(CPack)
