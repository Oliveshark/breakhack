cmake_minimum_required (VERSION 3.26)

SET(CMAKE_COLOR_MAKEFILE ON)

project(breakhack C)

set(breakhack_GAME_TITLE "BreakHack")

# Checksums
set(breakhack_STEAMAPI_DLL_CHECKSUM 0x1a4691a)
set(breakhack_STEAMAPI_SO_CHECKSUM 0x1a11b6d)

include(build_deps/cmake/FindCCache.cmake)
include(build_deps/cmake/FindCMocka.cmake)
include(build_deps/cmake/Findcppcheck.cmake)
include(build_deps/cmake/git_version.cmake)

get_version_from_git()
set(breakhack_MAJOR_VERSION ${PROJECT_VERSION_MAJOR})
set(breakhack_MINOR_VERSION ${PROJECT_VERSION_MINOR})
set(breakhack_PATCH_VERSION ${PROJECT_VERSION_PATCH})
set(breakhack_RELEASE_TYPE "")

configure_file(
	"${PROJECT_SOURCE_DIR}/src/config.h.in"
	"${PROJECT_BINARY_DIR}/config.h"
	)

set(DISABLE_STEAM_BUILD OFF CACHE BOOL "Disable Steam build")
set(STEAM OFF)

if (NOT DISABLE_STEAM_BUILD)
	if (EXISTS "${PROJECT_SOURCE_DIR}/lib/steamworks_c_wrapper/sdk")
		MESSAGE ( STATUS "Steam SDK located, Steam build enabled")
		set(STEAM ON)
	else ()
		MESSAGE ( STATUS "Steam SDK not found, Steam build disabled")
	endif()
else ()
	MESSAGE ( STATUS "Steam build disabled by DISABLE_STEAM_BUILD flag")
endif ()

if (STEAM)
	add_subdirectory(lib/steamworks_c_wrapper)
endif()
add_subdirectory(lib/bh_random)
add_subdirectory(lib/checksum)

if ("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
	set(CLANG 1)
elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
	set(GCC 1)
endif()

if ("${CMAKE_GENERATOR}" STREQUAL "Ninja")
	set(NINJA 1)
endif()

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
	set(OSX 1)
endif()

if (APPLE)
	enable_language(OBJC)
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
		string (REPLACE
			"/MDd"
			"/MTd"
			CMAKE_C_FLAGS_DEBUG
			${CMAKE_C_FLAGS_DEBUG}
    )
		string (REPLACE
			"/MDd"
			"/MTd"
			CMAKE_C_FLAGS_RELEASE
			${CMAKE_C_FLAGS_RELEASE}
    )
ELSEIF ( WIN32 )
    SET (
      CMAKE_C_FLAGS_DEBUG
      "${CMAKE_C_FLAGS_DEBUG} -mconsole"
    )
    SET (
      CMAKE_C_FLAGS_RELEASE
      "${CMAKE_C_FLAGS_RELEASE} -mwindows"
    )
ENDIF ()
IF ( GCC AND NOT MINGW )
	SET (
		CMAKE_C_FLAGS_DEBUG
		"${CMAKE_C_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer"
	)
ENDIF ()

if (STEAM)
	include_directories(
	lib/steamworks_c_wrapper/src
	)
endif ()

# Include SDL
set(SDL_STATIC ON)
set(SDL_SHARED OFF)
set(SDL_TEST OFF)
add_subdirectory(lib/SDL)
set(SDL_INCLUDE_DIR lib/SDL/include)
set(SDL_LIBRARY SDL3-static)

# Include SDL_image/mixer/ttf
set(BUILD_SHARED_LIBS OFF)

set(SDLIMAGE_PNG ON)
set(SDLIMAGE_JPG ON)
set(SDLIMAGE_AVIF OFF)
set(SDLIMAGE_BMP OFF)
set(SDLIMAGE_GIF OFF)
set(SDLIMAGE_JXL OFF)
set(SDLIMAGE_LBM OFF)
set(SDLIMAGE_PCX OFF)
set(SDLIMAGE_PNM OFF)
set(SDLIMAGE_QOI OFF)
set(SDLIMAGE_SVG OFF)
set(SDLIMAGE_TGA OFF)
set(SDLIMAGE_TIF OFF)
set(SDLIMAGE_WEBP OFF)
set(SDLIMAGE_XCF OFF)
set(SDLIMAGE_XPM OFF)
set(SDLIMAGE_XV OFF)
add_subdirectory(lib/SDL_image)
set(SDL_IMAGE_INCLUDE_DIR lib/SDL_image/include)
set(SDL_IMAGE_LIBRARY SDL3_image-static)

# opusfile caused issues in macos build and we don't need it anyways
set(SDLMIXER_OPUS OFF)
add_subdirectory(lib/SDL_mixer)
set(SDL_MIXER_INCLUDE_DIR lib/SDL_mixer/include)
set(SDL_MIXER_LIBRARY SDL3_mixer-static)

set(SDLTTF_VENDORED ON)
add_subdirectory(lib/SDL_ttf)
set(SDL_TTF_INCLUDE_DIR lib/SDL_ttf/include)
set(SDL_TTF_LIBRARY SDL3_ttf-static)

# Include lua lib
add_subdirectory(lib/lua-5.3.5)
set(LUA_LIBRARIES lua)
set(LUA_INCLUDE_DIR lib/lua-5.3.5/src)

# Include PHYSFS lib
set(PHYSFS_BUILD_SHARED off CACHE INTERNAL "" FORCE)
set(PHYSFS_BUILD_TEST off CACHE INTERNAL "" FORCE)
add_subdirectory(lib/physfs-3.0)
include_directories(lib/physfs-3.0/src)
set(PHYSFS_LIBRARY physfs-static)
set(PHYSFS_INCLUDE_DIR physfs-3.0/src)

if (CMOCKA_FOUND)
	include_directories(
		${CMOCKA_INCLUDE_DIR}
		)
endif(CMOCKA_FOUND)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")
if (STEAM)
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DSTEAM_BUILD")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DSTEAM_BUILD")
	set(STEAM_SOURCES
		src/steam/steamworks_api_wrapper.c
		)
else ()
	set(STEAM_SOURCES "")
endif ()

if (MINGW)
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DMINGW")
endif (MINGW)

if (NOT MSVC AND NOT NINJA)
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D__FNAME__='\"$(subst ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")
else ()
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D__FNAME__=__FILE__")
endif ()

set(INCLUDE_DIRS
	${PROJECT_BINARY_DIR}
	${SDL_INCLUDE_DIR}
	${SDL_IMAGE_INCLUDE_DIR}
	${SDL_TTF_INCLUDE_DIR}
	${SDL_MIXER_INCLUDE_DIR}
	${PHYSFS_INCLUDE_DIR}
	${LUA_INCLUDE_DIR}
	lib/sqlite3
	lib/bh_random/src
	lib/checksum/src
)
# PROGRAMS:
add_executable(${PROJECT_NAME})
add_subdirectory(src)
add_subdirectory(lib/sqlite3)
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 99)
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})

if (NOT MSVC)
	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS
		-std=gnu11 -pedantic -Wall -Wextra -Wshadow
		-Wpointer-arith -Wcast-qual -Wstrict-prototypes
		-Wmissing-prototypes -Wconversion -Wno-sign-conversion
	)
	if (NOT APPLE)
		add_definitions(-static -static-libgcc -static-libstdc++)
	endif ()
endif (NOT MSVC)


if (MSVC)
	source_group("Header files" REGULAR_EXPRESSION ".*\\.h")
	source_group("Source files" REGULAR_EXPRESSION ".*\\.c")
endif ()

# Sqlite has some warnings that I we don't need to see
set_source_files_properties(lib/sqlite3/sqlite3.c COMPILE_FLAGS -w)

target_link_libraries(${PROJECT_NAME}
	${CMAKE_DL_LIBS} # Sqlite needs DL libs
	${SDL_LIBRARY}
	${SDL_IMAGE_LIBRARY}
	${SDL_TTF_LIBRARY}
	${SDL_MIXER_LIBRARY}
	${LUA_LIBRARIES}
	${PHYSFS_LIBRARY}
	bh_random
	checksum
	)

if (STEAM)
	target_link_libraries(${PROJECT_NAME}
		${STEAMWORKS_LIBRARY}
		steamworks_c_wrapper
		)
endif ()

if (MSVC)
	set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE /NODEFAULTLIB:MSVCRTD")
	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_DEFINITIONS_DEBUG "_CONSOLE")
	set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:CONSOLE")
	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_DEFINITIONS_RELWITHDEBINFO "_CONSOLE")
	set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /NODEFAULTLIB:MSVCRTD")
	set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS")
endif (MSVC)

# TESTS:
IF (CMOCKA_FOUND AND NOT OSX AND NOT CLANG)
	find_package(Threads REQUIRED)
	enable_testing()
	add_executable(test_util test/test_util.c src/util.c)
	target_include_directories(test_util PRIVATE ${INCLUDE_DIRS})
	target_link_libraries(test_util ${CMOCKA_LIBRARY})
	add_test(test_util test_util)

	add_executable(test_linkedlist test/test_linkedlist.c src/linkedlist.c src/util.c)
	target_include_directories(test_linkedlist PRIVATE ${INCLUDE_DIRS})
	target_link_libraries(test_linkedlist ${CMOCKA_LIBRARY})
	add_test(test_linkedlist test_linkedlist)

	add_executable(test_hashtable test/test_hashtable.c src/hashtable.c src/util.c)
	target_include_directories(test_hashtable PRIVATE ${INCLUDE_DIRS})
	target_link_libraries(test_hashtable ${CMOCKA_LIBRARY})
	add_test(test_hashtable test_hashtable)

	add_executable(test_pos_heap test/test_pos_heap.c src/pos_heap.c src/util.c)
	target_include_directories(test_pos_heap PRIVATE ${INCLUDE_DIRS})
	target_link_libraries(test_pos_heap ${CMOCKA_LIBRARY})
	add_test(test_pos_heap test_pos_heap)

	add_executable(test_input test/test_input.c src/input.c src/keyboard.c)
	target_include_directories(test_input PRIVATE ${INCLUDE_DIRS})
	target_link_libraries(test_input
		${CMOCKA_LIBRARY}
		${SDL_LIBRARY}
		)
	add_test(test_input test_input)
ENDIF ()

# LINT:
if (CPPCHECK_FOUND)
	add_custom_target(lint
		COMMAND ${CPPCHECK_EXECUTABLE} --force --language=c --template=gcc --error-exitcode=1 --quiet --suppress=missingInclude --inline-suppr --enable=warning,style,performance,portability,information,missingInclude src/
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
			"trapgen.lua"
			"chestgen.lua"
			"layoutparser.lua"
			"pitlayouts.dat"
			"walllayouts.dat"
			"shoplayouts.dat"
			"lockedroomlayouts.dat"
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data
		)
endif ()

SET(CMAKE_INSTALL_SYSTEM_RUNTIME_COMPONENT "Release")
SET(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ".")
if (WIN32)
	if (STEAM)
		SET(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS
			${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
			lib/steamworks_c_wrapper/sdk/redistributable_bin/steam_api.dll
			)
	endif ()
else ()
	if (STEAM)
		SET(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS
			${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
			lib/steamworks_c_wrapper/sdk/redistributable_bin/linux64/libsteam_api.so
			build_deps/linux/breakhack.run
			)
	endif ()
endif ()
include(InstallRequiredSystemLibraries)

if (NOT STEAM)
	INSTALL(TARGETS ${PROJECT_NAME}
		COMPONENT Release
		RUNTIME DESTINATION bin/
	)
	INSTALL(FILES assets.pack data.pack
		COMPONENT Release
		DESTINATION share/breakhack/
	)
else ()
	INSTALL(TARGETS ${PROJECT_NAME}
		COMPONENT Release
		RUNTIME DESTINATION .
	)
	INSTALL(FILES assets.pack data.pack
		COMPONENT Release
		DESTINATION .
	)
endif ()

set(CPACK_INSTALL_CMAKE_PROJECTS "${PROJECT_BINARY_DIR};${PROJECT_NAME};Release;.")
set(CPACK_PACKAGE_NAME "BreakHack")
set(CPACK_PACKAGE_VENDOR "OliveShark")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_PACKAGE_DIRECTORY ${PROJECT_BINARY_DIR}/package)
set(CPACK_PACKAGE_VERSION_MAJOR ${breakhack_MAJOR_VERSION})
set(CPACK_PACKAGE_VERSION_MINOR ${breakhack_MINOR_VERSION})
set(CPACK_PACKAGE_VERSION_PATCH ${breakhack_PATCH_VERSION})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "BreakHack")
set(CPACK_PACKAGE_CHECKSUM "MD5")
if (WIN32)
	set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}-win32")
else ()
	set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
endif ()
if (STEAM)
	set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
	set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}-steambuild")
endif ()

if(UNIX)
	set(CPACK_GENERATOR TGZ ZIP)
	set(CPACK_STRIP_FILES ${PROJECT_NAME})
	set(CPACK_SOURCE_STRIP_FILES "")
elseif(WIN32)
	set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
	set(CPACK_GENERATOR NSIS ZIP)
endif()

include(CPack)
