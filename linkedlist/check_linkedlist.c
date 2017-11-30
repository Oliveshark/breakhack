#include <check.h>
#include <stdlib.h>
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
	int value = 14;
	LinkedList *list = linkedlist_create();
	ck_assert(linkedlist_size(list) == 0);
	linkedlist_append(&list, &value, sizeof(value));
	ck_assert(linkedlist_size(list) == 1);
	linkedlist_append(&list, &value, sizeof(value));
	ck_assert(linkedlist_size(list) == 2);
	linkedlist_destroy(&list);
	ck_assert( list == NULL );
}
END_TEST

START_TEST(test_linkedlist_poplast)
{
	int value1 = 1;
	int value2 = 2;
	int value3 = 3;

	int *pop1;
	int *pop2;
	int *pop3;

	LinkedList *list = linkedlist_create();

	ck_assert(linkedlist_size(list) == 0);

	linkedlist_append(&list, &value3, sizeof(int));
	linkedlist_append(&list, &value2, sizeof(int));
	linkedlist_append(&list, &value1, sizeof(int));

	ck_assert(linkedlist_size(list) == 3);

	pop1 = linkedlist_poplast(&list);
	pop2 = linkedlist_poplast(&list);
	pop3 = linkedlist_poplast(&list);

	ck_assert(linkedlist_size(list) == 0);

	ck_assert(*pop1 == value1);
	ck_assert(*pop2 == value2);
	ck_assert(*pop3 == value3);

	free(pop1);
	free(pop2);
	free(pop3);

	ck_assert(list == NULL);
}
END_TEST

START_TEST(test_linkedlist_push)
{
	int value = 14;
	LinkedList *list = linkedlist_create();
	ck_assert(linkedlist_size(list) == 0);
	linkedlist_push(&list, &value, sizeof(value));
	ck_assert(linkedlist_size(list) == 1);
	linkedlist_push(&list, &value, sizeof(value));
	ck_assert(linkedlist_size(list) == 2);
	linkedlist_destroy(&list);
	ck_assert( list == NULL );
}
END_TEST

START_TEST(test_linkedlist_pop)
{
	int value1 = 1;
	int value2 = 2;
	int value3 = 3;

	int *pop1;
	int *pop2;
	int *pop3;

	LinkedList *list = linkedlist_create();

	ck_assert(linkedlist_size(list) == 0);

	linkedlist_push(&list, &value3, sizeof(int));
	linkedlist_push(&list, &value2, sizeof(int));
	linkedlist_push(&list, &value1, sizeof(int));

	ck_assert(linkedlist_size(list) == 3);

	pop1 = linkedlist_pop(&list);
	pop2 = linkedlist_pop(&list);
	pop3 = linkedlist_pop(&list);

	ck_assert(linkedlist_size(list) == 0);

	ck_assert(*pop1 == value1);
	ck_assert(*pop2 == value2);
	ck_assert(*pop3 == value3);

	free(pop1);
	free(pop2);
	free(pop3);

	ck_assert(list == NULL);
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
