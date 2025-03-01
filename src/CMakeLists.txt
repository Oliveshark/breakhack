file(GLOB SOURCE_FILES CONFIGURE_DEPENDS *.c)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES})

if (STEAM)
	target_sources(${PROJECT_NAME} PRIVATE steam/steamworks_api_wrapper.c)
endif ()
