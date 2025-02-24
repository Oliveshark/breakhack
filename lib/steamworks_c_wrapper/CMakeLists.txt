cmake_minimum_required(VERSION 3.26)
project(steamworks_c_wrapper CXX)

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif ()

include(cmake/FindSTEAMWORKS.cmake)

if (BIT_32)
	MESSAGE("COMPILING 32 BIT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -m32")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_DEBUG} -m32")
endif()

set (Windows 0)
set (Apple 0)
if (WIN32)
	set(Windows 1)
elseif (APPLE)
	set(Apple 1)
else()
	#Linux
endif ()


add_library(steamworks_c_wrapper STATIC
	src/steamworks_c_wrapper.cpp
	src/CallbackHandler.cpp
	)
target_include_directories(steamworks_c_wrapper PRIVATE
	${STEAMWORKS_INCLUDE_DIR})

target_link_libraries(steamworks_c_wrapper ${STEAMWORKS_LIBRARY})
if (NOT MSVC)
	if (MINGW)
		target_link_libraries(steamworks_c_wrapper -static -lstdc++)
	else ()
		target_link_libraries(steamworks_c_wrapper -lstdc++)
	endif ()
endif()

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
