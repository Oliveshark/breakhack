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
#include "cmocka_include.h"
#include "../src/collisions.h"

static void test_position_in_rect(void **state)
{
	const SDL_Rect r = {0, 0, 2, 2};
	Position p;

	p = POS(1, 1);
	assert_true(position_in_rect(&p, &r));

	p = POS(2, 2);
	assert_false(position_in_rect(&p, &r));

	p = POS(0, 0);
	assert_true(position_in_rect(&p, &r));

	p = POS(2, 0);
	assert_false(position_in_rect(&p, &r));

	p = POS(0, 2);
	assert_false(position_in_rect(&p, &r));
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_position_in_rect),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
