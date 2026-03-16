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

#include "cmocka_include.h"
#include "../src/random.h"

/* set_random_seed / get_random_seed round-trip */
static void
test_seed_roundtrip(void **state)
{
	(void)state;

	set_random_seed(42);
	assert_int_equal(get_random_seed(), 42);

	set_random_seed(0xDEADBEEF);
	assert_int_equal(get_random_seed(), 0xDEADBEEF);
}

/* Same seed must produce identical map seeds for all 20 levels */
static void
test_map_seeds_are_deterministic(void **state)
{
	(void)state;

	unsigned int first[20], second[20];

	set_random_seed(12345);
	for (int i = 0; i < 20; ++i)
		first[i] = get_random_map_seed(i + 1);

	set_random_seed(12345);
	for (int i = 0; i < 20; ++i)
		second[i] = get_random_map_seed(i + 1);

	for (int i = 0; i < 20; ++i)
		assert_int_equal(first[i], second[i]);
}

/* Two different seeds must not produce the same map seed sequence */
static void
test_different_seeds_differ(void **state)
{
	(void)state;

	unsigned int a[20], b[20];

	set_random_seed(11111);
	for (int i = 0; i < 20; ++i)
		a[i] = get_random_map_seed(i + 1);

	set_random_seed(22222);
	for (int i = 0; i < 20; ++i)
		b[i] = get_random_map_seed(i + 1);

	/* At least one level must differ — all matching would be a collision */
	int any_differ = 0;
	for (int i = 0; i < 20; ++i) {
		if (a[i] != b[i]) {
			any_differ = 1;
			break;
		}
	}
	assert_true(any_differ);
}

/* get_random(max) must always return a value in [0, max] */
static void
test_get_random_bounds(void **state)
{
	(void)state;

	set_random_seed(99999);

	for (unsigned int max = 0; max <= 100; ++max) {
		for (int trial = 0; trial < 50; ++trial) {
			unsigned int r = get_random(max);
			assert_true(r <= max);
		}
	}
}

/*
 * Seed 0 is the "uninitialised" sentinel in init_seed().
 * set_random_seed(0) works fine, but get_random_seed() will subsequently
 * re-init with time(NULL) because init_seed() sees seed==0 as unset.
 * Verify that the code doesn't crash and that get_random_seed returns
 * something non-zero after that lazy re-init.
 */
static void
test_seed_zero_reinit(void **state)
{
	(void)state;

	set_random_seed(0);
	/* get_random_seed() triggers init_seed() which replaces 0 with time(NULL) */
	assert_int_not_equal(get_random_seed(), 0);
	/* map seeds are now populated from the time-based seed — must not crash */
	(void)get_random_map_seed(1);
	(void)get_random_map_seed(20);
}

int
main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_seed_roundtrip),
		cmocka_unit_test(test_map_seeds_are_deterministic),
		cmocka_unit_test(test_different_seeds_differ),
		cmocka_unit_test(test_get_random_bounds),
		cmocka_unit_test(test_seed_zero_reinit),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
