# - Try to find the steamworks library
#
# Once done, this will define:
#
#  STEAMWORKS_INCLUDE_DIR - the Steamworks include directory
#  STEAMWORKS_LIBRARIES - The libraries needed to use Steamworks

find_path(STEAMWORKS_INCLUDE_DIR
    NAMES
        steam_api.h
    PATHS
		${PROJECT_SOURCE_DIR}/sdk/public/steam/
)

find_library(STEAMWORKS_LIBRARY
	NAMES
		steam_api steam_api64
	PATHS
		${PROJECT_SOURCE_DIR}/sdk/redistributable_bin/win64/
		${PROJECT_SOURCE_DIR}/sdk/redistributable_bin/linux64/
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(STEAMWORKS DEFAULT_MSG STEAMWORKS_INCLUDE_DIR STEAMWORKS_LIBRARY)
