#include "table.h"

DEFINE_TEST(TEST_TABLE_ITER)
{
    ktable *table = ktable_new();
    CHECK_TEST(ktable_size(table) == 0);

    ktable_set(table, "key1", (void*)1);
    ktable_set(table, "key2", (void*)2);
    ktable_set(table, "key3", (void*)3);
    ktable_set(table, "foo1", (void*)4);
    ktable_set(table, "bar1", (void*)5);
    ktable_set(table, "kuma", (void*)6);
    CHECK_TEST(ktable_size(table) == 6);

    ktable_iter iter;
    ktable_iter_init(table, &iter);

    intptr_t val = 1;
    intptr_t total = 0;
    while(ktable_iter_next(&iter))
    {
        total += (intptr_t)ktable_iter_value(&iter);
        val++;
    }

    CHECK_TEST(val == 7);
    CHECK_TEST(total == 21);

    ktable_destroy(table);

    return 0;
}

DEFINE_TEST(TEST_TABLE_SETGET)
{
    ktable *table = ktable_new();
    CHECK_TEST(ktable_size(table) == 0);

    ktable_set(table, "key1", (void*)1);
    ktable_set(table, "key2", (void*)2);
    ktable_set(table, "key3", (void*)3);
    CHECK_TEST(ktable_size(table) == 3);
    CHECK_TEST((intptr_t)ktable_get(table, "key1") == 1);
    CHECK_TEST((intptr_t)ktable_get(table, "key2") == 2);
    CHECK_TEST((intptr_t)ktable_get(table, "key3") == 3);

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
