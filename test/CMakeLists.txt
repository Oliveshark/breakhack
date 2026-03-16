find_package(Threads REQUIRED)
enable_testing()

# Usage: add_test(NAME src1.c src2.c ...
#                [EXTRA_LIBS lib1 lib2]
#                [EXTRA_INCLUDES dir1 dir2]
#                [ENV "VAR=val;VAR2=val2"])
function(add_test TEST_NAME)
	cmake_parse_arguments(ARG "" "" "EXTRA_LIBS;EXTRA_INCLUDES;ENV" ${ARGN})
	message("Adding test: ${TEST_NAME} -> ${ARG_UNPARSED_ARGUMENTS}")
	add_executable(${TEST_NAME} ${ARG_UNPARSED_ARGUMENTS})
	target_include_directories(${TEST_NAME} PRIVATE ${INCLUDE_DIRS} ${ARG_EXTRA_INCLUDES})
	target_link_libraries(${TEST_NAME} ${CMOCKA_LIBRARY} ${SDL_LIBRARY} ${ARG_EXTRA_LIBS})
	_add_test(${TEST_NAME} ${TEST_NAME})
	if(ARG_ENV)
		set_tests_properties(${TEST_NAME} PROPERTIES ENVIRONMENT "${ARG_ENV}")
	endif()
endfunction()

add_test(test_util test_util.c ../src/util.c)
add_test(test_linkedlist test_linkedlist.c ../src/linkedlist.c ../src/util.c)
add_test(test_hashtable test_hashtable.c ../src/hashtable.c ../src/util.c)
add_test(test_pos_heap test_pos_heap.c ../src/pos_heap.c ../src/util.c)
add_test(test_input test_input.c ../src/input.c ../src/keyboard.c)
add_test(test_position test_position.c ../src/position.c)
add_test(test_collisions test_collisions.c ../src/collisions.c)

add_test(test_random test_random.c ../src/random.c ../src/util.c
	EXTRA_INCLUDES ${CMAKE_SOURCE_DIR}/lib/bh_random/src
	EXTRA_LIBS bh_random
)

# roommatrix needs a real SDL renderer (lightmap texture), so it uses the
# offscreen video driver. Extra libs: SDL_image + SDL_ttf (pulled in by
# texture.c) and physfs (pulled in by io_util.c / physfsrwops.c).
add_test(test_roommatrix
	test_roommatrix.c
	../src/roommatrix.c
	../src/texture.c
	../src/util.c
	../src/linkedlist.c
	../src/position.c
	../src/io_util.c
	../src/physfsrwops.c
	EXTRA_LIBS ${SDL_IMAGE_LIBRARY} ${SDL_TTF_LIBRARY} ${PHYSFS_LIBRARY}
	ENV "SDL_VIDEODRIVER=offscreen;SDL_RENDER_DRIVER=software"
)