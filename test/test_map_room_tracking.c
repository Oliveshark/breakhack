/*
 * BreakHack - A dungeone crawler RPG
 * Copyright (C) 2025  Linus Probert <linus.probert@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "cmocka_include.h"
#include "../src/map.h"
#include "../src/position.h"
#include "../src/defines.h"

/* Helper function to create a minimal map for testing */
static Map* create_test_map(void)
{
	Map *map = ec_malloc(sizeof(Map));
	memset(map, 0, sizeof(Map));
	
	/* Initialize all rooms */
	for (unsigned int i = 0; i < MAP_H_ROOM_COUNT; i++) {
		for (unsigned int j = 0; j < MAP_V_ROOM_COUNT; j++) {
			Room *room = ec_malloc(sizeof(Room));
			memset(room, 0, sizeof(Room));
			room->visited = false;
			map->rooms[i][j] = room;
		}
	}
	
	map->currentRoom = POS(0, 0);
	return map;
}

static void destroy_test_map(Map *map)
{
	if (\!map) return;
	
	for (unsigned int i = 0; i < MAP_H_ROOM_COUNT; i++) {
		for (unsigned int j = 0; j < MAP_V_ROOM_COUNT; j++) {
			if (map->rooms[i][j]) {
				free(map->rooms[i][j]);
			}
		}
	}
	free(map);
}

/* Test: Basic room coordinate calculation from world position */
static void test_map_set_current_room_basic(void **state)
{
	(void) state;
	
	Map *map = create_test_map();
	Position player_pos = POS(0, 0);
	bool first_visit = false;
	
	map_set_current_room(map, &player_pos, &first_visit);
	
	assert_int_equal(map->currentRoom.x, 0);
	assert_int_equal(map->currentRoom.y, 0);
	assert_true(first_visit);
	assert_true(map->rooms[0][0]->visited);
	
	destroy_test_map(map);
}

/* Test: First visit detection - should be true on first entry */
static void test_map_set_current_room_first_visit_true(void **state)
{
	(void) state;
	
	Map *map = create_test_map();
	Position player_pos = POS(
		MAP_ROOM_WIDTH * TILE_DIMENSION + 10,
		MAP_ROOM_HEIGHT * TILE_DIMENSION + 20
	);
	bool first_visit = false;
	
	/* Mark room as not visited */
	map->rooms[1][1]->visited = false;
	
	map_set_current_room(map, &player_pos, &first_visit);
	
	assert_int_equal(map->currentRoom.x, 1);
	assert_int_equal(map->currentRoom.y, 1);
	assert_true(first_visit);
	assert_true(map->rooms[1][1]->visited);
	
	destroy_test_map(map);
}

/* Test: First visit detection - should be false on subsequent entry */
static void test_map_set_current_room_first_visit_false(void **state)
{
	(void) state;
	
	Map *map = create_test_map();
	Position player_pos = POS(
		MAP_ROOM_WIDTH * TILE_DIMENSION * 2 + 15,
		MAP_ROOM_HEIGHT * TILE_DIMENSION * 3 + 25
	);
	bool first_visit = true;
	
	/* Mark room as already visited */
	map->rooms[2][3]->visited = true;
	
	map_set_current_room(map, &player_pos, &first_visit);
	
	assert_int_equal(map->currentRoom.x, 2);
	assert_int_equal(map->currentRoom.y, 3);
	assert_false(first_visit);
	assert_true(map->rooms[2][3]->visited);
	
	destroy_test_map(map);
}

/* Test: NULL first_visit parameter should not crash */
static void test_map_set_current_room_null_first_visit(void **state)
{
	(void) state;
	
	Map *map = create_test_map();
	Position player_pos = POS(50, 75);
	
	/* Should not crash with NULL first_visit */
	map_set_current_room(map, &player_pos, NULL);
	
	assert_int_equal(map->currentRoom.x, 0);
	assert_int_equal(map->currentRoom.y, 0);
	assert_true(map->rooms[0][0]->visited);
	
	destroy_test_map(map);
}

/* Test: Room boundary edge case - position at exact room boundary */
static void test_map_set_current_room_exact_boundary(void **state)
{
	(void) state;
	
	Map *map = create_test_map();
	unsigned int room_width = MAP_ROOM_WIDTH * TILE_DIMENSION;
	unsigned int room_height = MAP_ROOM_HEIGHT * TILE_DIMENSION;
	
	/* Position exactly at room (1,1) boundary */
	Position player_pos = POS(room_width, room_height);
	bool first_visit = false;
	
	map_set_current_room(map, &player_pos, &first_visit);
	
	assert_int_equal(map->currentRoom.x, 1);
	assert_int_equal(map->currentRoom.y, 1);
	
	destroy_test_map(map);
}

/* Test: Negative position (should clamp to 0,0) */
static void test_map_set_current_room_negative_position(void **state)
{
	(void) state;
	
	Map *map = create_test_map();
	Position player_pos = POS(-10, -20);
	bool first_visit = false;
	
	map_set_current_room(map, &player_pos, &first_visit);
	
	assert_int_equal(map->currentRoom.x, 0);
	assert_int_equal(map->currentRoom.y, 0);
	
	destroy_test_map(map);
}

/* Test: Position beyond map bounds (should clamp to max) */
static void test_map_set_current_room_beyond_bounds(void **state)
{
	(void) state;
	
	Map *map = create_test_map();
	unsigned int room_width = MAP_ROOM_WIDTH * TILE_DIMENSION;
	unsigned int room_height = MAP_ROOM_HEIGHT * TILE_DIMENSION;
	
	/* Position way beyond map boundaries */
	Position player_pos = POS(
		room_width * MAP_H_ROOM_COUNT + 1000,
		room_height * MAP_V_ROOM_COUNT + 2000
	);
	bool first_visit = false;
	
	map_set_current_room(map, &player_pos, &first_visit);
	
	assert_int_equal(map->currentRoom.x, MAP_H_ROOM_COUNT - 1);
	assert_int_equal(map->currentRoom.y, MAP_V_ROOM_COUNT - 1);
	
	destroy_test_map(map);
}

/* Test: Multiple rooms visited in sequence */
static void test_map_set_current_room_multiple_visits(void **state)
{
	(void) state;
	
	Map *map = create_test_map();
	unsigned int room_width = MAP_ROOM_WIDTH * TILE_DIMENSION;
	unsigned int room_height = MAP_ROOM_HEIGHT * TILE_DIMENSION;
	bool first_visit;
	
	/* Visit room (0,0) - first time */
	Position pos1 = POS(10, 10);
	map_set_current_room(map, &pos1, &first_visit);
	assert_true(first_visit);
	assert_true(map->rooms[0][0]->visited);
	
	/* Visit room (1,0) - first time */
	Position pos2 = POS(room_width + 10, 10);
	map_set_current_room(map, &pos2, &first_visit);
	assert_true(first_visit);
	assert_true(map->rooms[1][0]->visited);
	
	/* Revisit room (0,0) - not first time */
	map_set_current_room(map, &pos1, &first_visit);
	assert_false(first_visit);
	
	/* Visit room (2,1) - first time */
	Position pos3 = POS(room_width * 2 + 10, room_height + 10);
	map_set_current_room(map, &pos3, &first_visit);
	assert_true(first_visit);
	assert_true(map->rooms[2][1]->visited);
	
	destroy_test_map(map);
}

/* Test: Position within same room should maintain visited state */
static void test_map_set_current_room_same_room_movement(void **state)
{
	(void) state;
	
	Map *map = create_test_map();
	map->rooms[0][0]->visited = false;
	bool first_visit;
	
	/* First position in room (0,0) */
	Position pos1 = POS(50, 60);
	map_set_current_room(map, &pos1, &first_visit);
	assert_true(first_visit);
	
	/* Move within same room */
	Position pos2 = POS(100, 120);
	map_set_current_room(map, &pos2, &first_visit);
	assert_false(first_visit);
	assert_int_equal(map->currentRoom.x, 0);
	assert_int_equal(map->currentRoom.y, 0);
	
	destroy_test_map(map);
}

/* Test: Room coordinate calculation for various positions */
static void test_map_set_current_room_coordinate_calculation(void **state)
{
	(void) state;
	
	Map *map = create_test_map();
	unsigned int room_width = MAP_ROOM_WIDTH * TILE_DIMENSION;
	unsigned int room_height = MAP_ROOM_HEIGHT * TILE_DIMENSION;
	
	struct {
		Position player_pos;
		Position expected_room;
	} test_cases[] = {
		{ POS(0, 0), POS(0, 0) },
		{ POS(1, 1), POS(0, 0) },
		{ POS(room_width - 1, room_height - 1), POS(0, 0) },
		{ POS(room_width, room_height), POS(1, 1) },
		{ POS(room_width * 2, room_height * 2), POS(2, 2) },
		{ POS(room_width * 5 + 100, room_height * 3 + 200), POS(5, 3) },
	};
	
	for (size_t i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++) {
		map_set_current_room(map, &test_cases[i].player_pos, NULL);
		assert_int_equal(map->currentRoom.x, test_cases[i].expected_room.x);
		assert_int_equal(map->currentRoom.y, test_cases[i].expected_room.y);
	}
	
	destroy_test_map(map);
}

/* Test: Edge case - position at one pixel before room boundary */
static void test_map_set_current_room_one_pixel_before_boundary(void **state)
{
	(void) state;
	
	Map *map = create_test_map();
	unsigned int room_width = MAP_ROOM_WIDTH * TILE_DIMENSION;
	unsigned int room_height = MAP_ROOM_HEIGHT * TILE_DIMENSION;
	
	/* One pixel before crossing to next room */
	Position player_pos = POS(room_width - 1, room_height - 1);
	
	map_set_current_room(map, &player_pos, NULL);
	
	assert_int_equal(map->currentRoom.x, 0);
	assert_int_equal(map->currentRoom.y, 0);
	
	destroy_test_map(map);
}

/* Test: Corner rooms at map boundaries */
static void test_map_set_current_room_corner_rooms(void **state)
{
	(void) state;
	
	Map *map = create_test_map();
	unsigned int room_width = MAP_ROOM_WIDTH * TILE_DIMENSION;
	unsigned int room_height = MAP_ROOM_HEIGHT * TILE_DIMENSION;
	bool first_visit;
	
	/* Top-left corner (0, 0) */
	Position pos1 = POS(0, 0);
	map_set_current_room(map, &pos1, &first_visit);
	assert_int_equal(map->currentRoom.x, 0);
	assert_int_equal(map->currentRoom.y, 0);
	assert_true(first_visit);
	
	/* Top-right corner */
	Position pos2 = POS(
		room_width * (MAP_H_ROOM_COUNT - 1) + 10,
		10
	);
	map_set_current_room(map, &pos2, &first_visit);
	assert_int_equal(map->currentRoom.x, MAP_H_ROOM_COUNT - 1);
	assert_int_equal(map->currentRoom.y, 0);
	assert_true(first_visit);
	
	/* Bottom-left corner */
	Position pos3 = POS(
		10,
		room_height * (MAP_V_ROOM_COUNT - 1) + 10
	);
	map_set_current_room(map, &pos3, &first_visit);
	assert_int_equal(map->currentRoom.x, 0);
	assert_int_equal(map->currentRoom.y, MAP_V_ROOM_COUNT - 1);
	assert_true(first_visit);
	
	/* Bottom-right corner */
	Position pos4 = POS(
		room_width * (MAP_H_ROOM_COUNT - 1) + 10,
		room_height * (MAP_V_ROOM_COUNT - 1) + 10
	);
	map_set_current_room(map, &pos4, &first_visit);
	assert_int_equal(map->currentRoom.x, MAP_H_ROOM_COUNT - 1);
	assert_int_equal(map->currentRoom.y, MAP_V_ROOM_COUNT - 1);
	assert_true(first_visit);
	
	destroy_test_map(map);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_map_set_current_room_basic),
		cmocka_unit_test(test_map_set_current_room_first_visit_true),
		cmocka_unit_test(test_map_set_current_room_first_visit_false),
		cmocka_unit_test(test_map_set_current_room_null_first_visit),
		cmocka_unit_test(test_map_set_current_room_exact_boundary),
		cmocka_unit_test(test_map_set_current_room_negative_position),
		cmocka_unit_test(test_map_set_current_room_beyond_bounds),
		cmocka_unit_test(test_map_set_current_room_multiple_visits),
		cmocka_unit_test(test_map_set_current_room_same_room_movement),
		cmocka_unit_test(test_map_set_current_room_coordinate_calculation),
		cmocka_unit_test(test_map_set_current_room_one_pixel_before_boundary),
		cmocka_unit_test(test_map_set_current_room_corner_rooms),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}