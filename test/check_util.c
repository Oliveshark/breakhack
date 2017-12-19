#include <stdlib.h>
#include <check.h>
#include "../src/util.h"

START_TEST(test_util_ec_malloc)
{
	int *i = ec_malloc(sizeof(int));
	ck_assert( i != NULL );
	free(i);
}
END_TEST

static Suite* t_suite_create(void)
{
	Suite *s;
	TCase *tc_core;

	s = suite_create("Util");
	tc_core = tcase_create("Core");

	tcase_add_test(tc_core, test_util_ec_malloc);
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
