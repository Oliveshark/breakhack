/*
 * BreakHack - A dungeone crawler RPG
 * Copyright (C) 2026  Linus Probert <linus.probert@gmail.com>
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
#include "../src/roommatrix.h"
#include "../src/defines.h"

typedef struct {
	SDL_Window   *window;
	SDL_Renderer *renderer;
	RoomMatrix   *rm;
} TestState;

static int
setup(void **state)
{
	SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "offscreen");
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");

	if (!SDL_Init(SDL_INIT_VIDEO))
		return -1;

	SDL_Window *window = SDL_CreateWindow("test", 1, 1, 0);
	if (!window) {
		SDL_Quit();
		return -1;
	}

	SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
	if (!renderer) {
		SDL_DestroyWindow(window);
		SDL_Quit();
		return -1;
	}

	TestState *s = malloc(sizeof(TestState));
	s->window   = window;
	s->renderer = renderer;
	s->rm       = roommatrix_create(renderer);

	*state = s;
	return 0;
}

static int
teardown(void **state)
{
	TestState *s = *state;
	roommatrix_destroy(s->rm);
	SDL_DestroyRenderer(s->renderer);
	SDL_DestroyWindow(s->window);
	SDL_Quit();
	free(s);
	return 0;
}

/* Center of the matrix, all spaces open: self + 8 neighbours = 9 */
static void
test_surrounding_all_open(void **state)
{
	RoomMatrix *rm = ((TestState *)*state)->rm;

	Position center = POS(3 * TILE_DIMENSION, 3 * TILE_DIMENSION);
	Position out[9];
	size_t n = roommatrix_get_surrounding_spaces(rm, &center, out, 9);

	assert_int_equal(n, 9);
}

/* One occupied neighbour is excluded */
static void
test_surrounding_occupied_excluded(void **state)
{
	RoomMatrix *rm = ((TestState *)*state)->rm;

	SPACE_SET_FLAG(&rm->spaces[3][2], TILE_OCCUPIED);

	Position center = POS(3 * TILE_DIMENSION, 3 * TILE_DIMENSION);
	Position out[9];
	size_t n = roommatrix_get_surrounding_spaces(rm, &center, out, 9);

	assert_int_equal(n, 8);

	SPACE_CLEAR_FLAG(&rm->spaces[3][2], TILE_OCCUPIED);
}

/* A lethal tile is also not walkable and must be excluded */
static void
test_surrounding_lethal_excluded(void **state)
{
	RoomMatrix *rm = ((TestState *)*state)->rm;

	SPACE_SET_FLAG(&rm->spaces[3][4], TILE_LETHAL);

	Position center = POS(3 * TILE_DIMENSION, 3 * TILE_DIMENSION);
	Position out[9];
	size_t n = roommatrix_get_surrounding_spaces(rm, &center, out, 9);

	assert_int_equal(n, 8);

	SPACE_CLEAR_FLAG(&rm->spaces[3][4], TILE_LETHAL);
}

/* Corner (0,0): self + 3 in-bounds neighbours = 4 */
static void
test_surrounding_at_corner(void **state)
{
	RoomMatrix *rm = ((TestState *)*state)->rm;

	Position corner = POS(0, 0);
	Position out[9];
	size_t n = roommatrix_get_surrounding_spaces(rm, &corner, out, 9);

	assert_int_equal(n, 4);
}

int
main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test_setup_teardown(test_surrounding_all_open, setup, teardown),
		cmocka_unit_test_setup_teardown(test_surrounding_occupied_excluded, setup, teardown),
		cmocka_unit_test_setup_teardown(test_surrounding_lethal_excluded, setup, teardown),
		cmocka_unit_test_setup_teardown(test_surrounding_at_corner, setup, teardown),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
