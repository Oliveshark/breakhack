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

#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <cmocka.h>
#include "../src/hashtable.h"
#include "../src/util.h"

static void test_hashtable_create(void **state)
{
	(void) state;

	Hashtable *table = ht_create(10);
	assert_non_null( table );
	assert_int_equal(table->size, 10 );
	ht_destroy(table);
}

static void test_hashtable_set_get(void **state)
{
	(void) state;

	int i;
	int* values0[10];
	int* values1[10];
	char* keys[] = {
		"key0",
		"key1",
		"key2",
		"key3",
		"key4",
		"key5",
		"key6",
		"key7",
		"key8",
		"key9",
	};

	for (i = 0; i < 10; ++i) {
		values0[i] = malloc(sizeof(int));
		*values0[i] = i;

		values1[i] = malloc(sizeof(int));
		*values1[i] = 9-i;
	}

	Hashtable *table = ht_create(10);

	for (i = 0; i < 10; ++i) {
		ht_set(table, keys[i], values0[i]);
	}

	for (i = 0; i < 10; ++i) {
		assert_int_equal( *values0[i], *((int*) ht_get(table, keys[i])) );
	}

	for (i = 0; i < 10; ++i) {
		ht_set(table, keys[i], values1[i]);
	}

	for (i = 0; i < 10; ++i) {
		assert_int_equal( *values1[i], *((int*) ht_get(table, keys[i])) );
	}

	ht_destroy(table);
}

static bool checklist[] = {
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false
};

static void check_number(int *num)
{
	checklist[*num] = true;
}

static void test_hashtable_foreach(void **state)
{
	(void) state;

	Hashtable *table = ht_create(10);

	for (int i = 0; i < 10; i++) {
		char str[4];
		int *num = malloc(sizeof(int));
		assert_non_null( num );
		*num = i;
		m_sprintf(str, 4, "%d", *num);
		ht_set(table, str, num);
	}

	ht_foreach(table, (void (*)(void*)) check_number);
	for (int i = 0; i < 10; i++) {
		assert_non_null(checklist[i]);
	}

	ht_destroy(table);
}

static void test_hashtable_remove(void **state)
{
	(void) state;

	char key1[] = "key1";
	char value1[] = "value1";
	char key2[] = "key2";
	char value2[] = "value2";
	char key3[] = "key3";
	char value3[] = "value3";
	char *getVal;

	Hashtable *table = ht_create(10);
	ht_set(table, key1, value1);
	ht_set(table, key2, value2);
	ht_set(table, key3, value3);
	getVal = ht_remove(table, key2);
	assert_string_equal(value2, getVal);
	assert_null(ht_get(table, key2));
	ht_remove(table, key1);
	ht_remove(table, key3);

	ht_destroy(table);
}

int main(void)
{

	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_hashtable_create),
		cmocka_unit_test(test_hashtable_set_get),
		cmocka_unit_test(test_hashtable_remove),
		cmocka_unit_test(test_hashtable_foreach)
	};

	cmocka_run_group_tests(tests, NULL, NULL);
}
