#include <check.h>
#include <stdlib.h>
#include "hashtable.h"

START_TEST(test_hashtable_create)
{
	Hashtable *table = ht_create(10);
	ck_assert( table->size == 10 );
	ck_assert( table != NULL );
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

Suite* t_suite_create()
{
	Suite *s;
	TCase *tc_core;

	s = suite_create("Hashtable");
	tc_core = tcase_create("Core");

	tcase_add_test(tc_core, test_hashtable_create);
	tcase_add_test(tc_core, test_hashtable_set_get);
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
