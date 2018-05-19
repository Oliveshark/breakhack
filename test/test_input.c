/*
 * BreakHack - A dungeone crawler RPG
 * Copyright (C) 2018  Linus Probert <linus.probert@gmail.com>
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

#include <SDL.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <cmocka.h>
#include "../src/input.h"

static void
test_event_parse(void **state)
{
	(void) state;
	Input input;
	input_init(&input);

	SDL_KeyboardEvent event;
	event.type = SDL_KEYDOWN;
	event.keysym = (SDL_Keysym) { SDL_SCANCODE_W, SDLK_w, KMOD_NONE, 0 };

	input_handle_event(&input, (SDL_Event*) &event);
	event.keysym = (SDL_Keysym) { SDL_SCANCODE_0, SDLK_0, KMOD_NONE, 0 };
	input_handle_event(&input, (SDL_Event*) &event);

	assert_true(input_key_is_pressed(&input, KEY_UP));
	assert_true(input_key_is_pressed(&input, KEY_NUM0));

	event.type = SDL_KEYUP;
	event.keysym = (SDL_Keysym) { SDL_SCANCODE_0, SDLK_0, KMOD_NONE, 0 };
	input_handle_event(&input, (SDL_Event*) &event);

	assert_true(input_key_is_pressed(&input, KEY_UP));
	assert_true(!input_key_is_pressed(&input, KEY_NUM0));

	input_reset(&input);

	assert_true(input_key_is_released(&input, KEY_UP));
	assert_true(!input_key_is_released(&input, KEY_NUM0));
}

static void
test_keypress(void **state)
{
	(void) state;
	Input input;
	input_init(&input);
	input.lastKeyState = 0;
	input.keyState = KEY_UP;
	assert_true(input_key_is_pressed(&input, KEY_UP));
}

static void
test_keyrelease(void **state)
{
	(void) state;
	Input input;
	input_init(&input);
	input.lastKeyState = KEY_UP;
	input.keyState = 0;
	assert_true(input_key_is_released(&input, KEY_UP));
}

static void
test_keydown(void **state)
{
	(void) state;
	Input input;
	input_init(&input);
	input.keyState = KEY_UP;
	assert_true(input_key_is_down(&input, KEY_UP));
}

static void
test_mousebuttons(void **state)
{
	(void) state;
	Input input;
	input_init(&input);

	SDL_MouseButtonEvent event;
	event.type = SDL_MOUSEBUTTONDOWN;
	event.button = SDL_BUTTON_LEFT;
	input_handle_event(&input, (SDL_Event*) &event);

	assert_true(input_mousebutton_is_pressed(&input, MBUTTON_LEFT));

	input_reset(&input);
	event.button = SDL_BUTTON_RIGHT;
	input_handle_event(&input, (SDL_Event*) &event);

	assert_true(!input_mousebutton_is_pressed(&input, MBUTTON_LEFT));
	assert_true(input_mousebutton_is_pressed(&input, MBUTTON_RIGHT));
}

int main(int argc, char *argv[])
{
	(void) argc;
	(void) argv;

	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_keypress),
		cmocka_unit_test(test_keyrelease),
		cmocka_unit_test(test_keydown),
		cmocka_unit_test(test_event_parse),
		cmocka_unit_test(test_mousebuttons),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
