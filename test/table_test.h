#include "table.h"

DEFINE_TEST(TEST_TABLE_ITER)
{
    ktable *table = ktable_new();
    CHECK_TEST(ktable_size(table) == 0);

    ktable_set(table, "key1", 1);
    ktable_set(table, "key2", 2);
    ktable_set(table, "key3", 3);
    CHECK_TEST(ktable_size(table) == 3);

    ktable_iter iter;
    ktable_iter_init(table, &iter);

    int val = 1;
    while(ktable_iter_next(&iter))
    {
        CHECK_TEST(ktable_iter_value(&iter) == val);
        val++;
    }

    CHECK_TEST(val == 4);

    ktable_destroy(table);

    return 0;
}

DEFINE_TEST(TEST_TABLE_SETGET)
{
    ktable *table = ktable_new();
    CHECK_TEST(ktable_size(table) == 0);

    ktable_set(table, "key1", 1);
    ktable_set(table, "key2", 2);
    ktable_set(table, "key3", 3);
    CHECK_TEST(ktable_size(table) == 3);
    CHECK_TEST(ktable_get(table, "key1") == 1);
    CHECK_TEST(ktable_get(table, "key2") == 2);
    CHECK_TEST(ktable_get(table, "key3") == 3);

    ktable_remove(table, "key1");
    CHECK_TEST(ktable_size(table) == 2);
    CHECK_TEST(ktable_get(table, "key1") == NULL);

    ktable_remove(table, "key2");
    CHECK_TEST(ktable_size(table) == 1);
    CHECK_TEST(ktable_get(table, "key2") == NULL);

    ktable_remove(table, "key3");
    CHECK_TEST(ktable_size(table) == 0);
    CHECK_TEST(ktable_get(table, "key3") == NULL);

    ktable_destroy(table);

    return 0;
}
