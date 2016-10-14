#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "test.h"
#include "list_test.h"
#include "table_test.h"

int main()
{
    RUN_TEST(TEST_LIST_ADD);
    RUN_TEST(TEST_LIST_ADD_FIRST);
    RUN_TEST(TEST_LIST_ADD_LAST);
    RUN_TEST(TEST_LIST_ADD_AT);
    RUN_TEST(TEST_LIST_REMOVE_ALL);
    RUN_TEST(TEST_LIST_REMOVE);
    RUN_TEST(TEST_LIST_REMOVE_AT);
    RUN_TEST(TEST_LIST_REMOVE_FIRST);
    RUN_TEST(TEST_LIST_REMOVE_LAST);

    RUN_TEST(TEST_TABLE_ITER);
    RUN_TEST(TEST_TABLE_SETGET);
}
