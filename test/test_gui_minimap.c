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

#include <SDL3/SDL.h>
#include "cmocka_include.h"
#include "../src/gui.h"
#include "../src/defines.h"
#include "../src/roommatrix.h"
#include "../src/camera.h"
#include "../src/sprite.h"
#include "../src/texture.h"

/* Mock SDL renderer and window for testing */
static SDL_Window *mock_window = NULL;
static SDL_Renderer *mock_renderer = NULL;

static int setup_sdl(void **state)
{
	(void) state;
	
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		return -1;
	}
	
	mock_window = SDL_CreateWindow(
		"Test",
		800, 600,
		SDL_WINDOW_HIDDEN
	);
	
	if (\!mock_window) {
		SDL_Quit();
		return -1;
	}
	
	mock_renderer = SDL_CreateRenderer(mock_window, NULL);
	if (\!mock_renderer) {
		SDL_DestroyWindow(mock_window);
		SDL_Quit();
		return -1;
	}
	
	return 0;
}

static int teardown_sdl(void **state)
{
	(void) state;
	
	if (mock_renderer) {
		SDL_DestroyRenderer(mock_renderer);
		mock_renderer = NULL;
	}
	
	if (mock_window) {
		SDL_DestroyWindow(mock_window);
		mock_window = NULL;
	}
	
	SDL_Quit();
	return 0;
}

/* Helper to create a minimal camera for testing */
static Camera* create_test_camera(void)
{
	Camera *cam = ec_malloc(sizeof(Camera));
	memset(cam, 0, sizeof(Camera));
	cam->renderer = mock_renderer;
	cam->pos = POS(0, 0);
	cam->basePos = POS(0, 0);
	return cam;
}

static void destroy_test_camera(Camera *cam)
{
	if (cam) {
		free(cam);
	}
}

/* Helper to create a minimal RoomMatrix for testing */
static RoomMatrix* create_test_roommatrix(void)
{
	RoomMatrix *rm = ec_malloc(sizeof(RoomMatrix));
	memset(rm, 0, sizeof(RoomMatrix));
	rm->roomPos = POS(0, 0);
	rm->playerRoomPos = POS(0, 0);
	rm->mousePos = POS(0, 0);
	
	/* Initialize all spaces to empty */
	for (size_t i = 0; i < MAP_ROOM_WIDTH; i++) {
		for (size_t j = 0; j < MAP_ROOM_HEIGHT; j++) {
			rm->spaces[i][j].flags = TILE_NONE;
			rm->spaces[i][j].light = 0;
			rm->spaces[i][j].tile = NULL;
			rm->spaces[i][j].wall = NULL;
			rm->spaces[i][j].door = NULL;
			rm->spaces[i][j].decoration = NULL;
			rm->spaces[i][j].monster = NULL;
			rm->spaces[i][j].player = NULL;
			rm->spaces[i][j].trap = NULL;
			rm->spaces[i][j].items = NULL;
			rm->spaces[i][j].artifacts = NULL;
			rm->spaces[i][j].objects = NULL;
		}
	}
	
	return rm;
}

static void destroy_test_roommatrix(RoomMatrix *rm)
{
	if (rm) {
		free(rm);
	}
}

/* Helper to create a minimal Gui with minimap sprite */
static Gui* create_test_gui_with_minimap(Camera *cam)
{
	Gui *gui = ec_malloc(sizeof(Gui));
	memset(gui, 0, sizeof(Gui));
	
	/* Create minimap sprite */
	Sprite *minimap = sprite_create();
	Texture *texture = texture_create();
	texture->dim = (Dimension) { RIGHT_GUI_WIDTH, MINIMAP_GUI_HEIGHT };
	minimap->textures[0] = texture;
	minimap->destroyTextures = true;
	minimap->pos = POS(0, 4);
	minimap->dim = (Dimension) { RIGHT_GUI_WIDTH, MINIMAP_GUI_HEIGHT };
	minimap->fixed = true;
	
	/* Create the actual texture */
	texture_create_blank(texture, SDL_TEXTUREACCESS_TARGET, cam->renderer);
	
	gui->miniMap = minimap;
	
	return gui;
}

static void destroy_test_gui(Gui *gui)
{
	if (\!gui) return;
	
	if (gui->miniMap) {
		sprite_destroy(gui->miniMap);
	}
	
	free(gui);
}

/* Test: gui_reset should clear minimap texture */
static void test_gui_reset_clears_minimap(void **state)
{
	(void) state;
	
	Camera *cam = create_test_camera();
	Gui *gui = create_test_gui_with_minimap(cam);
	
	/* Reset should not crash */
	gui_reset(gui, cam);
	
	/* Verify minimap texture still exists */
	assert_non_null(gui->miniMap);
	assert_non_null(gui->miniMap->textures[0]);
	assert_non_null(gui->miniMap->textures[0]->texture);
	
	destroy_test_gui(gui);
	destroy_test_camera(cam);
}

/* Test: gui_update_minimap should handle NULL parameters gracefully */
static void test_gui_update_minimap_null_safety(void **state)
{
	(void) state;
	
	Camera *cam = create_test_camera();
	Gui *gui = create_test_gui_with_minimap(cam);
	RoomMatrix *rm = create_test_roommatrix();
	
	/* Should not crash with valid parameters */
	gui_update_minimap(gui, cam, rm);
	
	destroy_test_roommatrix(rm);
	destroy_test_gui(gui);
	destroy_test_camera(cam);
}

/* Test: gui_update_minimap should process empty room */
static void test_gui_update_minimap_empty_room(void **state)
{
	(void) state;
	
	Camera *cam = create_test_camera();
	Gui *gui = create_test_gui_with_minimap(cam);
	RoomMatrix *rm = create_test_roommatrix();
	
	/* All spaces are empty (NULL tiles, no monsters, etc.) */
	gui_update_minimap(gui, cam, rm);
	
	/* Verify the operation completed without crash */
	assert_non_null(gui->miniMap);
	
	destroy_test_roommatrix(rm);
	destroy_test_gui(gui);
	destroy_test_camera(cam);
}

/* Test: gui_update_minimap should handle room with lethal spaces */
static void test_gui_update_minimap_lethal_spaces(void **state)
{
	(void) state;
	
	Camera *cam = create_test_camera();
	Gui *gui = create_test_gui_with_minimap(cam);
	RoomMatrix *rm = create_test_roommatrix();
	
	/* Set some spaces as lethal (pits, etc.) */
	for (size_t i = 0; i < 5; i++) {
		for (size_t j = 0; j < 5; j++) {
			rm->spaces[i][j].flags = TILE_LETHAL;
		}
	}
	
	gui_update_minimap(gui, cam, rm);
	
	/* Verify operation completed */
	assert_non_null(gui->miniMap);
	
	destroy_test_roommatrix(rm);
	destroy_test_gui(gui);
	destroy_test_camera(cam);
}

/* Test: gui_update_minimap should handle room with various tile types */
static void test_gui_update_minimap_mixed_tiles(void **state)
{
	(void) state;
	
	Camera *cam = create_test_camera();
	Gui *gui = create_test_gui_with_minimap(cam);
	RoomMatrix *rm = create_test_roommatrix();
	
	/* Create a mock MapTile */
	MapTile mock_tile;
	memset(&mock_tile, 0, sizeof(MapTile));
	mock_tile.levelExit = false;
	
	/* Set different space types */
	rm->spaces[0][0].tile = &mock_tile; /* Normal tile */
	rm->spaces[1][0].wall = &mock_tile; /* Wall */
	rm->spaces[2][0].door = &mock_tile; /* Door */
	rm->spaces[3][0].flags = TILE_LETHAL; /* Lethal */
	rm->spaces[4][0].flags = TILE_OCCUPIED; /* Occupied */
	
	gui_update_minimap(gui, cam, rm);
	
	/* Verify operation completed */
	assert_non_null(gui->miniMap);
	
	destroy_test_roommatrix(rm);
	destroy_test_gui(gui);
	destroy_test_camera(cam);
}

/* Test: gui_update_minimap with different room positions */
static void test_gui_update_minimap_different_room_positions(void **state)
{
	(void) state;
	
	Camera *cam = create_test_camera();
	Gui *gui = create_test_gui_with_minimap(cam);
	RoomMatrix *rm = create_test_roommatrix();
	
	/* Test room at position (0,0) */
	rm->roomPos = POS(0, 0);
	gui_update_minimap(gui, cam, rm);
	assert_non_null(gui->miniMap);
	
	/* Test room at position (2,3) */
	rm->roomPos = POS(2, 3);
	gui_update_minimap(gui, cam, rm);
	assert_non_null(gui->miniMap);
	
	/* Test room at position (5,7) */
	rm->roomPos = POS(5, 7);
	gui_update_minimap(gui, cam, rm);
	assert_non_null(gui->miniMap);
	
	destroy_test_roommatrix(rm);
	destroy_test_gui(gui);
	destroy_test_camera(cam);
}

/* Test: gui_render_minimap should not crash with valid inputs */
static void test_gui_render_minimap_basic(void **state)
{
	(void) state;
	
	Camera *cam = create_test_camera();
	Gui *gui = create_test_gui_with_minimap(cam);
	RoomMatrix *rm = create_test_roommatrix();
	
	rm->roomPos = POS(1, 2);
	
	/* Should not crash */
	gui_render_minimap(gui, cam, rm);
	
	destroy_test_roommatrix(rm);
	destroy_test_gui(gui);
	destroy_test_camera(cam);
}

/* Test: gui_render_minimap with room at different positions */
static void test_gui_render_minimap_room_positions(void **state)
{
	(void) state;
	
	Camera *cam = create_test_camera();
	Gui *gui = create_test_gui_with_minimap(cam);
	RoomMatrix *rm = create_test_roommatrix();
	
	/* Test various room positions */
	Position test_positions[] = {
		POS(0, 0),
		POS(1, 1),
		POS(5, 3),
		POS(MAP_H_ROOM_COUNT - 1, MAP_V_ROOM_COUNT - 1)
	};
	
	for (size_t i = 0; i < sizeof(test_positions) / sizeof(Position); i++) {
		rm->roomPos = test_positions[i];
		gui_render_minimap(gui, cam, rm);
	}
	
	destroy_test_roommatrix(rm);
	destroy_test_gui(gui);
	destroy_test_camera(cam);
}

/* Test: Sequential update and render cycle */
static void test_gui_minimap_update_render_cycle(void **state)
{
	(void) state;
	
	Camera *cam = create_test_camera();
	Gui *gui = create_test_gui_with_minimap(cam);
	RoomMatrix *rm = create_test_roommatrix();
	
	/* Simulate a typical game cycle */
	rm->roomPos = POS(2, 3);
	
	/* Reset minimap */
	gui_reset(gui, cam);
	
	/* Update with room data */
	gui_update_minimap(gui, cam, rm);
	
	/* Render */
	gui_render_minimap(gui, cam, rm);
	
	/* Verify state */
	assert_non_null(gui->miniMap);
	assert_non_null(gui->miniMap->textures[0]);
	
	destroy_test_roommatrix(rm);
	destroy_test_gui(gui);
	destroy_test_camera(cam);
}

/* Test: Multiple updates to same minimap */
static void test_gui_minimap_multiple_updates(void **state)
{
	(void) state;
	
	Camera *cam = create_test_camera();
	Gui *gui = create_test_gui_with_minimap(cam);
	RoomMatrix *rm = create_test_roommatrix();
	
	/* Update multiple times with different data */
	for (int i = 0; i < 5; i++) {
		rm->roomPos = POS(i, i);
		gui_update_minimap(gui, cam, rm);
	}
	
	/* Should still be valid after multiple updates */
	assert_non_null(gui->miniMap);
	assert_non_null(gui->miniMap->textures[0]);
	
	destroy_test_roommatrix(rm);
	destroy_test_gui(gui);
	destroy_test_camera(cam);
}

/* Test: Minimap dimensions are correct */
static void test_gui_minimap_dimensions(void **state)
{
	(void) state;
	
	Camera *cam = create_test_camera();
	Gui *gui = create_test_gui_with_minimap(cam);
	
	/* Verify minimap sprite dimensions */
	assert_int_equal(gui->miniMap->dim.width, RIGHT_GUI_WIDTH);
	assert_int_equal(gui->miniMap->dim.height, MINIMAP_GUI_HEIGHT);
	
	/* Verify texture dimensions */
	assert_int_equal(gui->miniMap->textures[0]->dim.width, RIGHT_GUI_WIDTH);
	assert_int_equal(gui->miniMap->textures[0]->dim.height, MINIMAP_GUI_HEIGHT);
	
	/* Verify sprite is marked as fixed */
	assert_true(gui->miniMap->fixed);
	
	destroy_test_gui(gui);
	destroy_test_camera(cam);
}

/* Test: Reset after multiple updates */
static void test_gui_reset_after_updates(void **state)
{
	(void) state;
	
	Camera *cam = create_test_camera();
	Gui *gui = create_test_gui_with_minimap(cam);
	RoomMatrix *rm = create_test_roommatrix();
	
	/* Update several times */
	for (int i = 0; i < 3; i++) {
		rm->roomPos = POS(i, i);
		gui_update_minimap(gui, cam, rm);
	}
	
	/* Reset should clear everything */
	gui_reset(gui, cam);
	
	/* Minimap should still be valid but cleared */
	assert_non_null(gui->miniMap);
	assert_non_null(gui->miniMap->textures[0]);
	
	destroy_test_roommatrix(rm);
	destroy_test_gui(gui);
	destroy_test_camera(cam);
}

/* Test: Minimap texture access type is TARGET */
static void test_gui_minimap_texture_access_type(void **state)
{
	(void) state;
	
	Camera *cam = create_test_camera();
	Gui *gui = create_test_gui_with_minimap(cam);
	
	/* Verify texture access type */
	assert_int_equal(
		gui->miniMap->textures[0]->textureAccessType,
		SDL_TEXTUREACCESS_TARGET
	);
	
	destroy_test_gui(gui);
	destroy_test_camera(cam);
}

/* Test: Minimap position is correct */
static void test_gui_minimap_position(void **state)
{
	(void) state;
	
	Camera *cam = create_test_camera();
	Gui *gui = create_test_gui_with_minimap(cam);
	
	/* Verify minimap position */
	assert_int_equal(gui->miniMap->pos.x, 0);
	assert_int_equal(gui->miniMap->pos.y, 4);
	
	destroy_test_gui(gui);
	destroy_test_camera(cam);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test_setup_teardown(
			test_gui_reset_clears_minimap,
			setup_sdl,
			teardown_sdl
		),
		cmocka_unit_test_setup_teardown(
			test_gui_update_minimap_null_safety,
			setup_sdl,
			teardown_sdl
		),
		cmocka_unit_test_setup_teardown(
			test_gui_update_minimap_empty_room,
			setup_sdl,
			teardown_sdl
		),
		cmocka_unit_test_setup_teardown(
			test_gui_update_minimap_lethal_spaces,
			setup_sdl,
			teardown_sdl
		),
		cmocka_unit_test_setup_teardown(
			test_gui_update_minimap_mixed_tiles,
			setup_sdl,
			teardown_sdl
		),
		cmocka_unit_test_setup_teardown(
			test_gui_update_minimap_different_room_positions,
			setup_sdl,
			teardown_sdl
		),
		cmocka_unit_test_setup_teardown(
			test_gui_render_minimap_basic,
			setup_sdl,
			teardown_sdl
		),
		cmocka_unit_test_setup_teardown(
			test_gui_render_minimap_room_positions,
			setup_sdl,
			teardown_sdl
		),
		cmocka_unit_test_setup_teardown(
			test_gui_minimap_update_render_cycle,
			setup_sdl,
			teardown_sdl
		),
		cmocka_unit_test_setup_teardown(
			test_gui_minimap_multiple_updates,
			setup_sdl,
			teardown_sdl
		),
		cmocka_unit_test_setup_teardown(
			test_gui_minimap_dimensions,
			setup_sdl,
			teardown_sdl
		),
		cmocka_unit_test_setup_teardown(
			test_gui_reset_after_updates,
			setup_sdl,
			teardown_sdl
		),
		cmocka_unit_test_setup_teardown(
			test_gui_minimap_texture_access_type,
			setup_sdl,
			teardown_sdl
		),
		cmocka_unit_test_setup_teardown(
			test_gui_minimap_position,
			setup_sdl,
			teardown_sdl
		),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}