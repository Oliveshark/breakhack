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
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <cmocka.h>
#include "../src/position.h"
#include "../src/defines.h"

#define assert_pos_eq(a, b) \
	do { \
		assert_int_equal(a.x, b.x); \
		assert_int_equal(a.y, b.y); \
	} while (0)

static void test_position_equals(void **state)
{
	Position a = POS(1, 1);
	Position b = POS(1, 1);
	Position c = POS(1, 2);

	assert_true(position_equals(&a, &b));
	assert_false(position_equals(&a, &c));
}

static void test_matrix_coords_conversion(void **state)
{
	Position pos;
	Position mc;
	Position expected;

	pos = POS(1, 1);
	expected = POS(0, 0);
	mc = position_to_matrix_coords(&pos);
	assert_pos_eq(mc, expected);

	pos = POS(TILE_DIMENSION * 5, TILE_DIMENSION * 5);
	expected = POS(5, 5);
	mc = position_to_matrix_coords(&pos);
	assert_pos_eq(mc, expected);

	pos = POS(TILE_DIMENSION * 5 + 1, TILE_DIMENSION * 5 + 1);
	expected = POS(5, 5);
	mc = position_to_matrix_coords(&pos);
	assert_pos_eq(mc, expected);

	pos = POS(TILE_DIMENSION * 5 + TILE_DIMENSION - 1, TILE_DIMENSION * 5 + TILE_DIMENSION - 1);
	expected = POS(5, 5);
	mc = position_to_matrix_coords(&pos);
	assert_pos_eq(mc, expected);

	pos = POS(TILE_DIMENSION * 5 + TILE_DIMENSION, TILE_DIMENSION * 5 + TILE_DIMENSION);
	expected = POS(6, 6);
	mc = position_to_matrix_coords(&pos);
	assert_pos_eq(mc, expected);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_position_equals),
		cmocka_unit_test(test_matrix_coords_conversion),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
