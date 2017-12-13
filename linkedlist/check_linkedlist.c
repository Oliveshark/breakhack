#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include "linkedlist.h"

START_TEST(test_linkedlist_create)
{
	LinkedList *list = linkedlist_create();
	ck_assert( list == NULL );
	linkedlist_destroy(&list);
}
END_TEST

START_TEST(test_linkedlist_append)
{
	int *v1, *v2;

	v1 = malloc(sizeof(int));
	v2 = malloc(sizeof(int));

	*v1 = 34;
	*v2 = 44;

	LinkedList *list = linkedlist_create();
	ck_assert(linkedlist_size(list) == 0);
	linkedlist_append(&list, v1);
	ck_assert(linkedlist_size(list) == 1);
	linkedlist_append(&list, v2);
	ck_assert(linkedlist_size(list) == 2);
	linkedlist_destroy(&list);
	ck_assert( list == NULL );
}
END_TEST

START_TEST(test_linkedlist_poplast)
{
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

	ck_assert(linkedlist_size(list) == 0);

	linkedlist_append(&list, v3);
	linkedlist_append(&list, v2);
	linkedlist_append(&list, v1);

	ck_assert(linkedlist_size(list) == 3);

	pop1 = linkedlist_poplast(&list);
	pop2 = linkedlist_poplast(&list);
	pop3 = linkedlist_poplast(&list);

	ck_assert(linkedlist_size(list) == 0);

	ck_assert(*pop1 == *v1);
	ck_assert(*pop2 == *v2);
	ck_assert(*pop3 == *v3);

	free(pop1);
	free(pop2);
	free(pop3);

	linkedlist_destroy(&list);

	ck_assert(list == NULL);
}
END_TEST

START_TEST(test_linkedlist_push)
{
	int *v1, *v2;

	v1 = malloc(sizeof(int));
	v2 = malloc(sizeof(int));

	*v1 = 1;
	*v2 = 1;

	LinkedList *list = linkedlist_create();
	ck_assert(linkedlist_size(list) == 0);
	linkedlist_push(&list, v1);
	ck_assert(linkedlist_size(list) == 1);
	linkedlist_push(&list, v2);
	ck_assert(linkedlist_size(list) == 2);
	linkedlist_destroy(&list);
	ck_assert( list == NULL );
}
END_TEST

START_TEST(test_linkedlist_pop)
{
	int *value1, *value2, *value3;
	int *pop1, *pop2, *pop3;

	value1 = malloc(sizeof(int));
	value2 = malloc(sizeof(int));
	value3 = malloc(sizeof(int));

	*value1 = 1;
	*value2 = 2;
	*value3 = 3;

	LinkedList *list = linkedlist_create();

	ck_assert(linkedlist_size(list) == 0);

	linkedlist_push(&list, value3);
	linkedlist_push(&list, value2);
	linkedlist_push(&list, value1);

	ck_assert(linkedlist_size(list) == 3);

	pop1 = linkedlist_pop(&list);
	pop2 = linkedlist_pop(&list);
	pop3 = linkedlist_pop(&list);

	ck_assert(linkedlist_size(list) == 0);

	ck_assert(*pop1 == *value1);
	ck_assert(*pop2 == *value2);
	ck_assert(*pop3 == *value3);

	linkedlist_destroy(&list);

	free(pop1);
	free(pop2);
	free(pop3);

	ck_assert(list == NULL);
}
END_TEST

START_TEST(test_linkedlist_get_index)
{
	int *value1, *value2, *get;

	value1 = malloc(sizeof(int));
	value2 = malloc(sizeof(int));
	
	*value1 = 1;
	*value2 = 2;


	LinkedList *list = linkedlist_create();

	ck_assert(linkedlist_size(list) == 0);

	linkedlist_push(&list, value2);
	linkedlist_push(&list, value1);

	ck_assert(linkedlist_size(list) == 2);

	get = linkedlist_get(&list, 1);

	ck_assert(linkedlist_size(list) == 2);

	ck_assert(*get == *value2);
	ck_assert(get == value2);

	linkedlist_destroy(&list);

	ck_assert(list == NULL);
}
END_TEST

static void
increase(int *number)
{
	(*number)++;
}

START_TEST (test_linkedlist_each)
{
	LinkedList *list;
	int *append;
	int i;

	list = linkedlist_create();

	for (i = 0; i < 10; ++i) {
		append = malloc(sizeof(int));
		*append = i;
		linkedlist_append(&list, append);
	}

	linkedlist_each(&list, (void (*)(void*)) increase);

	for (i = 0; i < 10; ++i) {
		int *pop = linkedlist_pop(&list);
		ck_assert( i+1 == *pop );
		free(pop);
		pop = NULL;
	}

	linkedlist_destroy(&list);
}
END_TEST

Suite* t_suite_create()
{
	Suite *s;
	TCase *tc_core;

	s = suite_create("LinkedList");
	tc_core = tcase_create("Core");

	tcase_add_test(tc_core, test_linkedlist_create);
	tcase_add_test(tc_core, test_linkedlist_push);
	tcase_add_test(tc_core, test_linkedlist_pop);
	tcase_add_test(tc_core, test_linkedlist_append);
	tcase_add_test(tc_core, test_linkedlist_poplast);
	tcase_add_test(tc_core, test_linkedlist_get_index);
	tcase_add_test(tc_core, test_linkedlist_each);
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
