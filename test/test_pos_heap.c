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
#include "../src/pos_heap.h"
#include "../src/util.h"

static void test_heap_create(void **state)
{
	PHeap ph;
	pheap_init(&ph, 42);
	assert_int_equal(ph.capacity, 42);
	assert_int_equal(ph.size, 0);

	pheap_destroy(&ph);
}

static void test_heap_insert(void **state)
{
	PHeap ph;
	pheap_init(&ph, 0);

	assert_int_equal(ph.capacity, 1);

	pheap_insert(&ph, (Position) { 12, 12 }, 1);
	pheap_insert(&ph, (Position) { 13, 12 }, 2);
	pheap_insert(&ph, (Position) { 14, 12 }, 3);
	pheap_insert(&ph, (Position) { 15, 12 }, 4);

	assert_int_equal(ph.size, 4);
	assert_int_equal(ph.capacity, 4);

	Position p;

	p = pheap_pop(&ph);
	assert_int_equal(p.x, 12);
	assert_int_equal(p.y, 12);
	assert_int_equal(ph.size, 3);

	p = pheap_pop(&ph);
	assert_int_equal(p.x, 13);
	assert_int_equal(p.y, 12);
	assert_int_equal(ph.size, 2);

	p = pheap_pop(&ph);
	assert_int_equal(p.x, 14);
	assert_int_equal(p.y, 12);
	assert_int_equal(ph.size, 1);

	p = pheap_pop(&ph);
	assert_int_equal(p.x, 15);
	assert_int_equal(p.y, 12);
	assert_int_equal(ph.size, 0);

	assert_int_equal(ph.capacity, 4);

	pheap_destroy(&ph);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_heap_create),
		cmocka_unit_test(test_heap_insert),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
