find_package(Threads REQUIRED)
enable_testing()

function(add_test TEST_NAME)
	message("Adding test: ${TEST_NAME} -> ${ARGN}")
	add_executable(${TEST_NAME} ${ARGN})
	target_include_directories(${TEST_NAME} PRIVATE ${INCLUDE_DIRS})
	target_link_libraries(${TEST_NAME} ${CMOCKA_LIBRARY} ${SDL_LIBRARY})
	_add_test(${TEST_NAME} ${TEST_NAME})
endfunction()

add_test(test_util test_util.c ../src/util.c)
add_test(test_linkedlist test_linkedlist.c ../src/linkedlist.c ../src/util.c)
add_test(test_hashtable test_hashtable.c ../src/hashtable.c ../src/util.c)
add_test(test_pos_heap test_pos_heap.c ../src/pos_heap.c ../src/util.c)
add_test(test_input test_input.c ../src/input.c ../src/keyboard.c)
add_test(test_position test_position.c ../src/position.c)
add_test(test_collisions test_collisions.c ../src/collisions.c)

add_test(test_random test_random.c ../src/random.c ../src/util.c)
target_include_directories(test_random PRIVATE ${CMAKE_SOURCE_DIR}/lib/bh_random/src)
target_link_libraries(test_random bh_random)

# roommatrix needs a real SDL renderer (lightmap texture), so it uses the
# offscreen video driver. Extra libs: SDL_image + SDL_ttf (pulled in by
# texture.c) and physfs (pulled in by io_util.c / physfsrwops.c).
add_executable(test_roommatrix
	test_roommatrix.c
	../src/roommatrix.c
	../src/texture.c
	../src/util.c
	../src/linkedlist.c
	../src/position.c
	../src/io_util.c
	../src/physfsrwops.c
)
target_include_directories(test_roommatrix PRIVATE ${INCLUDE_DIRS})
target_link_libraries(test_roommatrix
	${CMOCKA_LIBRARY}
	${SDL_LIBRARY}
	${SDL_IMAGE_LIBRARY}
	${SDL_TTF_LIBRARY}
	${PHYSFS_LIBRARY}
)
_add_test(test_roommatrix test_roommatrix)
set_tests_properties(test_roommatrix PROPERTIES
	ENVIRONMENT "SDL_VIDEODRIVER=offscreen;SDL_RENDER_DRIVER=software"
)
