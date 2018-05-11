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
#include "../src/linkedlist.h"

static void test_linkedlist_create(void **state)
{
	(void) state;

	LinkedList *list = linkedlist_create();
	assert_null( list );
	linkedlist_destroy(&list);
}

static void test_linkedlist_append(void **state)
{
	(void) state;

	int *v1, *v2;

	v1 = malloc(sizeof(int));
	v2 = malloc(sizeof(int));

	*v1 = 34;
	*v2 = 44;

	LinkedList *list = linkedlist_create();
	assert_int_equal(linkedlist_size(list), 0);
	linkedlist_append(&list, v1);
	assert_int_equal(linkedlist_size(list), 1);
	linkedlist_append(&list, v2);
	assert_int_equal(linkedlist_size(list), 2);
	linkedlist_destroy(&list);
	assert_int_equal( list, NULL );
}

static void test_linkedlist_poplast(void **state)
{
	(void) state;

	int *v1, *v2, *v3;

	v1 = malloc(sizeof(int));
	v2 = malloc(sizeof(int));
	v3 = malloc(sizeof(int));

	*v1 = 1;
	*v2 = 2;
	*v3 = 3;

	int *pop1;
	int *pop2;
	int *pop3;

	LinkedList *list = linkedlist_create();

	assert_int_equal(linkedlist_size(list), 0);

	linkedlist_append(&list, v3);
	linkedlist_append(&list, v2);
	linkedlist_append(&list, v1);

	assert_int_equal(linkedlist_size(list), 3);

	pop1 = linkedlist_poplast(&list);
	pop2 = linkedlist_poplast(&list);
	pop3 = linkedlist_poplast(&list);

	assert_int_equal(linkedlist_size(list), 0);

	assert_int_equal(*pop1, *v1);
	assert_int_equal(*pop2, *v2);
	assert_int_equal(*pop3, *v3);

	free(pop1);
	free(pop2);
	free(pop3);

	linkedlist_destroy(&list);

	assert_null(list);
}

static void test_linkedlist_push(void **state)
{
	(void) state;

	int *v1, *v2;

	v1 = malloc(sizeof(int));
	v2 = malloc(sizeof(int));

	*v1 = 1;
	*v2 = 1;

	LinkedList *list = linkedlist_create();
	assert_int_equal(linkedlist_size(list), 0);
	linkedlist_push(&list, v1);
	assert_int_equal(linkedlist_size(list), 1);
	linkedlist_push(&list, v2);
	assert_int_equal(linkedlist_size(list), 2);
	linkedlist_destroy(&list);
	assert_null( list );
}

static void test_linkedlist_pop(void **state)
{
	(void) state;

	int *value1, *value2, *value3;
	int *pop1, *pop2, *pop3;

	value1 = malloc(sizeof(int));
	value2 = malloc(sizeof(int));
	value3 = malloc(sizeof(int));

	*value1 = 1;
	*value2 = 2;
	*value3 = 3;

	LinkedList *list = linkedlist_create();

	assert_int_equal(linkedlist_size(list), 0);

	linkedlist_push(&list, value3);
	linkedlist_push(&list, value2);
	linkedlist_push(&list, value1);

	assert_int_equal(linkedlist_size(list), 3);

	pop1 = linkedlist_pop(&list);
	pop2 = linkedlist_pop(&list);
	pop3 = linkedlist_pop(&list);

	assert_int_equal(linkedlist_size(list), 0);

	assert_int_equal(*pop1, *value1);
	assert_int_equal(*pop2, *value2);
	assert_int_equal(*pop3, *value3);

	linkedlist_destroy(&list);

	free(pop1);
	free(pop2);
	free(pop3);

	assert_null(list );
}

static void test_linkedlist_get_index(void **state)
{
	(void) state;

	int *value1, *value2, *get;

	value1 = malloc(sizeof(int));
	value2 = malloc(sizeof(int));
	
	*value1 = 1;
	*value2 = 2;

	LinkedList *list = linkedlist_create();

	assert_int_equal(linkedlist_size(list), 0);

	linkedlist_push(&list, value2);
	linkedlist_push(&list, value1);

	assert_int_equal(linkedlist_size(list), 2);

	get = linkedlist_get(&list, 1);

	assert_int_equal(linkedlist_size(list), 2);

	assert_int_equal(*get, *value2);
	assert_int_equal(get, value2);

	linkedlist_destroy(&list);

	assert_null(list);
}

static void
increase(int *number)
{
	(*number)++;
}

static void test_linkedlist_each(void **state)
{
	(void) state;

	LinkedList *list;
	int i;

	list = linkedlist_create();

	for (i = 0; i < 10; ++i) {
		int *append = malloc(sizeof(int));
		*append = i;
		linkedlist_append(&list, append);
	}

	linkedlist_each(&list, (void (*)(void*)) increase);

	for (i = 0; i < 10; ++i) {
		int *pop = linkedlist_pop(&list);
		assert_int_equal( i+1, *pop );
		free(pop);
		pop = NULL;
	}

	linkedlist_destroy(&list);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_linkedlist_create),
		cmocka_unit_test(test_linkedlist_push),
		cmocka_unit_test(test_linkedlist_pop),
		cmocka_unit_test(test_linkedlist_append),
		cmocka_unit_test(test_linkedlist_poplast),
		cmocka_unit_test(test_linkedlist_get_index),
		cmocka_unit_test(test_linkedlist_each)
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
