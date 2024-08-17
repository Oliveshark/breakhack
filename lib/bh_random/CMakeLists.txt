cmake_minimum_required(VERSION 3.6)
project(bh_random CXX)

add_definitions(-std=c++11)

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif ()

add_library(bh_random STATIC
	src/bh_random.cpp
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

