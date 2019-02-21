cmake_minimum_required(VERSION 3.1)
project(bh_random)

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif ()

add_library(bh_random STATIC
	src/bh_random
	)

IF ( MSVC )
    MESSAGE ( STATUS "Setting MSVC MT switches")
    SET (
      CMAKE_CXX_FLAGS_DEBUG
        "${CMAKE_CXX_FLAGS_DEBUG} /MTd"
        CACHE STRING "MSVC MT flags " FORCE
    )
    SET (
      CMAKE_CXX_FLAGS_RELEASE
        "${CMAKE_CXX_FLAGS_RELEASE} /MT"
        CACHE STRING "MSVC MT flags " FORCE
    )
endif ()

