#include <zephyr/ztest.h>
#include "sum.h"

ZTEST_SUITE(sum_suite, NULL, NULL, NULL, NULL, NULL);

ZTEST(sum_suite, test_add_positive)
{
    zassert_equal(add(2, 3), 5, "2 + 3 should be 5");
}

ZTEST(sum_suite, test_add_negative)
{
    zassert_equal(add(-1, -1), -2, "-1 + -1 should be -2");
}

