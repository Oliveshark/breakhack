cmake_minimum_required(VERSION 3.6)
project(checksum C)

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif ()


add_executable(checksumtool
	src/checksum.c
	)
target_compile_definitions(checksumtool PUBLIC EXECUTABLE=1)

add_library(checksum
	src/checksum.c
	)

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
endif ()

