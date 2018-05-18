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

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <cmocka.h>
#include "../src/keyboardinput.h"
#include "../src/keyboard.h"


bool __wrap_keyboard_direction_press(Direction, SDL_Event*);
bool __wrap_keyboard_press(Uint64, SDL_Event*);

bool
__wrap_keyboard_direction_press(Direction d, SDL_Event *event)
{
	(void) d;
	(void) event;
	return mock_type(bool);
}

bool
__wrap_keyboard_press(Uint64 key, SDL_Event *event)
{
	(void) key;
	(void) event;
	return mock_type(bool);
}

static void
test_event_parse(void **state)
{
	(void) state;
	KeyboardInput input = { 0, 0 };

	will_return(__wrap_keyboard_direction_press, true);	// KEY_UP
	will_return(__wrap_keyboard_press, true);		// NUM0
	will_return(__wrap_keyboard_press, false);		// NUM1
	will_return(__wrap_keyboard_press, false);		// NUM2
	will_return(__wrap_keyboard_press, false);		// NUM3
	will_return(__wrap_keyboard_press, false);		// NUM4
	will_return(__wrap_keyboard_press, false);		// NUM5
	will_return(__wrap_keyboard_press, false);		// NUM6
	will_return(__wrap_keyboard_press, false);		// NUM7
	will_return(__wrap_keyboard_press, false);		// NUM8
	will_return(__wrap_keyboard_press, false);		// NUM9

	SDL_Event event;
	keyboardinput_handle_event(&input, &event);

	assert_true(key_is_pressed(&input, KEY_UP));
	assert_true(key_is_pressed(&input, KEY_NUM0));
}

static void
test_keypress(void **state)
{
	(void) state;
	KeyboardInput input = { 0, 0 };
	input.lastState = 0;
	input.currentState = KEY_UP;
	assert_true(key_is_pressed(&input, KEY_UP));
}

static void
test_keyrelease(void **state)
{
	(void) state;
	KeyboardInput input = { 0, 0 };
	input.lastState = KEY_UP;
	input.currentState = 0;
	assert_true(key_is_released(&input, KEY_UP));
}

static void
test_keydown(void **state)
{
	(void) state;
	KeyboardInput input = { 0, 0 };
	input.currentState = KEY_UP;
	assert_true(key_is_down(&input, KEY_UP));
}

int main(void)
{

	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_keypress),
		cmocka_unit_test(test_keyrelease),
		cmocka_unit_test(test_keydown),
		cmocka_unit_test(test_event_parse),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
