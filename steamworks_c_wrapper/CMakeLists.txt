cmake_minimum_required(VERSION 3.1)
project(steamworks_c_wrapper)

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

include_directories(${STEAMWORKS_INCLUDE_DIR})

add_library(steamworks_c_wrapper
	src/steamworks_c_wrapper
	)

target_link_libraries(steamworks_c_wrapper ${STEAMWORKS_LIBRARY})
