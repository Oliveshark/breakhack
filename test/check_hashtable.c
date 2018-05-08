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

#include <check.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "../src/hashtable.h"

START_TEST(test_hashtable_create)
{
	Hashtable *table = ht_create(10);
	ck_assert( table != NULL && table->size == 10 );
	ht_destroy(table);
}
END_TEST

START_TEST(test_hashtable_set_get)
{
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
		ck_assert( *values0[i] == *((int*) ht_get(table, keys[i])) );
	}

	for (i = 0; i < 10; ++i) {
		ht_set(table, keys[i], values1[i]);
	}

	for (i = 0; i < 10; ++i) {
		ck_assert( *values1[i] == *((int*) ht_get(table, keys[i])) );
	}

	ht_destroy(table);
}
END_TEST

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

START_TEST(test_hashtable_foreach)
{
	Hashtable *table = ht_create(10);

	for (int i = 0; i < 10; i++) {
		char str[4];
		int *num = malloc(sizeof(int));
		ck_assert( num != NULL );
		*num = i;
		sprintf(str, "%d", *num);
		ht_set(table, str, num);
	}

	ht_foreach(table, (void (*)(void*)) check_number);
	for (int i = 0; i < 10; i++) {
		ck_assert(checklist[i]);
	}

	ht_destroy(table);
}
END_TEST

START_TEST(test_hashtable_remove)
{
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
	ck_assert(strcmp(value2, getVal) == 0);
	ck_assert(ht_get(table, key2) == NULL);
	ht_remove(table, key1);
	ht_remove(table, key3);

	ht_destroy(table);
}
END_TEST

static Suite* t_suite_create(void)
{
	Suite *s;
	TCase *tc_core;

	s = suite_create("Hashtable");
	tc_core = tcase_create("Core");

	tcase_add_test(tc_core, test_hashtable_create);
	tcase_add_test(tc_core, test_hashtable_set_get);
	tcase_add_test(tc_core, test_hashtable_foreach);
	tcase_add_test(tc_core, test_hashtable_remove);
	suite_add_tcase(s, tc_core);

	return s;
}

int main(void)
{
	int number_failed;
	Suite *s;
	SRunner *sr;

	s = t_suite_create();
	sr = srunner_create(s);

	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);

	srunner_free(sr);

	return number_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
