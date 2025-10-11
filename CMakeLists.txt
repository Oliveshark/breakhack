cmake_minimum_required (VERSION 3.26)

SET(CMAKE_COLOR_MAKEFILE ON)

project(breakhack C)

set(breakhack_GAME_TITLE "BreakHack")

# CMake options
set(DISABLE_STEAM_BUILD OFF CACHE BOOL "Disable Steam build")
set(RELEASE_TYPE "" CACHE STRING "Release type")
set(STEAM_BUILD OFF CACHE STRING "Steam build")
set(PROFILER OFF CACHE STRING "Enable profiler")

# Checksums, Disabled until further notice
# set(breakhack_STEAMAPI_DLL_CHECKSUM 0x1a4691a)
# set(breakhack_STEAMAPI_SO_CHECKSUM 0x1a11b6d)

include(build_deps/cmake/FindCCache.cmake)
include(build_deps/cmake/FindCMocka.cmake)
include(build_deps/cmake/Findcppcheck.cmake)
include(build_deps/cmake/git_version.cmake)

get_version_from_git()
set(breakhack_MAJOR_VERSION ${PROJECT_VERSION_MAJOR})
set(breakhack_MINOR_VERSION ${PROJECT_VERSION_MINOR})
set(breakhack_PATCH_VERSION ${PROJECT_VERSION_PATCH})
set(breakhack_VERSION ${PROJECT_VERSION})
set(breakhack_FULL_VERSION ${FULL_VERSION})
set(breakhack_RELEASE_TYPE ${RELEASE_TYPE})

configure_file(
	"${PROJECT_SOURCE_DIR}/src/config.h.in"
	"${PROJECT_BINARY_DIR}/config.h"
	)


if (NOT DISABLE_STEAM_BUILD)
	if (EXISTS "${PROJECT_SOURCE_DIR}/lib/steamworks_c_wrapper/sdk")
		MESSAGE ( STATUS "Steam SDK located, Steam build enabled")
		set(STEAM_BUILD ON)
	else ()
		MESSAGE ( STATUS "Steam SDK not found, Steam build disabled")
	endif()
else ()
	MESSAGE ( STATUS "Steam build disabled by DISABLE_STEAM_BUILD flag")
endif ()

if (STEAM_BUILD)
	add_subdirectory(lib/steamworks_c_wrapper)
endif()
add_subdirectory(lib/bh_random)
add_subdirectory(lib/checksum)
if (PROFILER)
	add_subdirectory(lib/profiler)
endif()

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
	set(PACKAGE_COMPONENT "Beta")
else()
	set(PACKAGE_COMPONENT "Release")
endif()

if ( MSVC )
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
elseif ( WIN32 )
    set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -mconsole")
    set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -mwindows")
elseif ( GCC AND NOT MINGW )
	set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer")
	set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2")
elseif (EMSCRIPTEN)
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -flto -O3 -fno-exceptions -fno-rtti")
endif ()

if (STEAM_BUILD)
	include_directories(
	lib/steamworks_c_wrapper/src
	)
endif ()

# Include SDL
set(SDL_STATIC ON)
set(SDL_SHARED OFF)
set(SDL_TEST OFF)
add_subdirectory(external/SDL)
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
add_subdirectory(external/SDL_image SYSTEM)
set(SDL_IMAGE_LIBRARY SDL3_image-static)

set(SDLMIXER_OPUS OFF)
set(SDLMIXER_AIFF OFF)
set(SDLMIXER_VOC OFF)
set(SDLMIXER_AU OFF)
set(SDLMIXER_GME OFF)
set(SDLMIXER_MIDI_TIMIDITY OFF)
set(SDLMIXER_MOD_XMP OFF)
set(SDLMIXER_MP3_DRMP3 OFF)
set(SDLMIXER_MP3_MPG123 OFF)
set(SDLMIXER_WAVPACK OFF)
set(SDLMIXER_VORBIS_VORBISFILE_ENABLED OFF)
add_subdirectory(external/SDL_mixer SYSTEM)
set(SDL_MIXER_LIBRARY SDL3_mixer-static)

set(SDLTTF_VENDORED ON)
add_subdirectory(external/SDL_ttf SYSTEM)
set(SDL_TTF_LIBRARY SDL3_ttf-static)

# Include lua lib
add_subdirectory(lib/lua-5.3.5 SYSTEM)
set(LUA_LIBRARIES lua)
set(LUA_INCLUDE_DIR lib/lua-5.3.5/src)

# Include PHYSFS lib
set(PHYSFS_BUILD_SHARED off)
set(PHYSFS_BUILD_TEST off)
add_subdirectory(lib/physfs-3.0 SYSTEM)
include_directories(lib/physfs-3.0/src)
set(PHYSFS_LIBRARY physfs-static)
set(PHYSFS_INCLUDE_DIR physfs-3.0/src)

if (CMOCKA_FOUND)
	include_directories(
		${CMOCKA_INCLUDE_DIR}
		)
endif(CMOCKA_FOUND)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")
if (STEAM_BUILD)
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DSTEAM_BUILD")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DSTEAM_BUILD")
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
	PRIVATE ${PROJECT_BINARY_DIR}
	PRIVATE lib/sqlite3
	PRIVATE lib/bh_random/src
	PRIVATE lib/checksum/src
	SYSTEM ${PHYSFS_INCLUDE_DIR}
	SYSTEM ${LUA_INCLUDE_DIR}
)
# PROGRAMS:
add_executable(${PROJECT_NAME})
add_subdirectory(src)
add_subdirectory(lib/sqlite3)
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 11)
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})

if (PROFILER)
	add_definitions("-DPROFILER")
	target_link_libraries(${PROJECT_NAME} profiler)
	target_include_directories(${PROJECT_NAME} PRIVATE lib/profiler/include)
endif ()

if (NOT MSVC)
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Wconversion -Wno-sign-conversion")
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
	bh_random
	checksum
	${SDL_LIBRARY}
	${SDL_IMAGE_LIBRARY}
	${SDL_TTF_LIBRARY}
	${SDL_MIXER_LIBRARY}
	${LUA_LIBRARIES}
	${PHYSFS_LIBRARY}
)

if (STEAM_BUILD)
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
IF (CMOCKA_FOUND AND NOT OSX)
	add_subdirectory(test)
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

SET(CMAKE_INSTALL_SYSTEM_RUNTIME_COMPONENT "${PACKAGE_COMPONENT}")
SET(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ".")
if (STEAM_BUILD)
	if (WIN32)
		SET(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS
			${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
			lib/steamworks_c_wrapper/sdk/redistributable_bin/win64/steam_api64.dll
		)
	else ()
		SET(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS
			${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
			lib/steamworks_c_wrapper/sdk/redistributable_bin/linux64/libsteam_api.so
			build_deps/linux/breakhack.run
			)
	endif ()
endif ()
include(InstallRequiredSystemLibraries)

if (NOT STEAM_BUILD)
	INSTALL(TARGETS ${PROJECT_NAME}
		COMPONENT Release
		RUNTIME DESTINATION bin/
	)
	INSTALL(TARGETS ${PROJECT_NAME}
		COMPONENT Beta
		RUNTIME DESTINATION bin/
	)
	INSTALL(FILES assets.pack data.pack
		COMPONENT Release
		DESTINATION share/breakhack/
	)
	INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_CURRENT_SOURCE_DIR}/data
		COMPONENT Beta
		DESTINATION share/breakhack/
	)
else ()
	INSTALL(TARGETS ${PROJECT_NAME}
		COMPONENT Release
		RUNTIME DESTINATION .
	)
	INSTALL(TARGETS ${PROJECT_NAME}
		COMPONENT Beta
		RUNTIME DESTINATION .
	)
	INSTALL(FILES assets.pack data.pack
		COMPONENT Release
		DESTINATION .
	)
	INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_CURRENT_SOURCE_DIR}/data
		COMPONENT Beta
		DESTINATION .
	)
endif ()

set(CPACK_INSTALL_CMAKE_PROJECTS "${PROJECT_BINARY_DIR};${PROJECT_NAME};${PACKAGE_COMPONENT};.")
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
if (STEAM_BUILD)
	set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
	set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}-steambuild")
endif ()

if(UNIX)
	set(CPACK_GENERATOR TGZ ZIP)
	set(CPACK_STRIP_FILES ${PROJECT_NAME})
	set(CPACK_SOURCE_STRIP_FILES "")
elseif(WIN32)
	set(CPACK_GENERATOR ZIP)
endif()

include(CPack)
