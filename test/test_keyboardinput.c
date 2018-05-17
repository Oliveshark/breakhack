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

static KeyboardInput input = { 0, 0 };

static void
test_keypress(void **state)
{
	(void) state;
	input.lastState = 0;
	input.currentState = KEY_UP;
	assert_true(key_is_pressed(&input, KEY_UP));
}

static void
test_keyrelease(void **state)
{
	(void) state;
	input.lastState = KEY_UP;
	input.currentState = 0;
	assert_true(key_is_released(&input, KEY_UP));
}

static void
test_keydown(void **state)
{
	(void) state;
	input.currentState = KEY_UP;
	assert_true(key_is_down(&input, KEY_UP));
}

int main(void)
{

	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_keypress),
		cmocka_unit_test(test_keyrelease),
		cmocka_unit_test(test_keydown),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
