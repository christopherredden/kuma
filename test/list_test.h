#include "list.h"

DEFINE_TEST(TEST_LIST_ADD_FIRST)
{
    klist *int_list = klist_new();
    CHECK_TEST(klist_size(int_list) == 0);

    klist_add_first(int_list, 1);
    CHECK_TEST(klist_size(int_list) == 1);
    CHECK_TEST(klist_get_at(int, int_list, 0) == 1);

    klist_add_first(int_list, 2);
    CHECK_TEST((int)klist_size(int_list) == 2);
    CHECK_TEST(klist_get_at(int, int_list, 0) == 2);
    CHECK_TEST(klist_get_at(int, int_list, 1) == 1);

    klist_add_first(int_list, 3);
    CHECK_TEST(klist_size(int_list) == 3);
    CHECK_TEST(klist_get_at(int, int_list, 0) == 3);
    CHECK_TEST(klist_get_at(int, int_list, 1) == 2);
    CHECK_TEST(klist_get_at(int, int_list, 2) == 1);

    klist_destroy(int_list);

    return 0;
}

DEFINE_TEST(TEST_LIST_ADD)
{
    klist *int_list = klist_new();
    CHECK_TEST(klist_size(int_list) == 0);

    klist_add(int_list, 1);
    CHECK_TEST(klist_size(int_list) == 1);
    CHECK_TEST(klist_get_at(int, int_list, 0) == 1);

    klist_add(int_list, 2);
    CHECK_TEST((int)klist_size(int_list) == 2);
    CHECK_TEST(klist_get_at(int, int_list, 0) == 1);
    CHECK_TEST(klist_get_at(int, int_list, 1) == 2);

    klist_add(int_list, 3);
    CHECK_TEST(klist_size(int_list) == 3);
    CHECK_TEST(klist_get_at(int, int_list, 0) == 1);
    CHECK_TEST(klist_get_at(int, int_list, 1) == 2);
    CHECK_TEST(klist_get_at(int, int_list, 2) == 3);

    klist_destroy(int_list);

    return 0;
}

DEFINE_TEST(TEST_LIST_ADD_LAST)
{
    klist *int_list = klist_new();
    CHECK_TEST(klist_size(int_list) == 0);

    klist_add_last(int_list, 1);
    CHECK_TEST(klist_size(int_list) == 1);
    CHECK_TEST(klist_get_at(int, int_list, 0) == 1);

    klist_add_last(int_list, 2);
    CHECK_TEST((int)klist_size(int_list) == 2);
    CHECK_TEST(klist_get_at(int, int_list, 0) == 1);
    CHECK_TEST(klist_get_at(int, int_list, 1) == 2);

    klist_add_last(int_list, 3);
    CHECK_TEST(klist_size(int_list) == 3);
    CHECK_TEST(klist_get_at(int, int_list, 0) == 1);
    CHECK_TEST(klist_get_at(int, int_list, 1) == 2);
    CHECK_TEST(klist_get_at(int, int_list, 2) == 3);

    klist_destroy(int_list);

    return 0;
}

DEFINE_TEST(TEST_LIST_ADD_AT)
{
    klist *int_list = klist_new();
    CHECK_TEST(klist_size(int_list) == 0);

    klist_add(int_list, 1);
    klist_add(int_list, 2);
    klist_add(int_list, 3);
    klist_add_at(int_list, 4, 1);

    CHECK_TEST(klist_size(int_list) == 4);
    CHECK_TEST(klist_get_at(int, int_list, 0) == 1);
    CHECK_TEST(klist_get_at(int, int_list, 1) == 4);
    CHECK_TEST(klist_get_at(int, int_list, 2) == 2);
    CHECK_TEST(klist_get_at(int, int_list, 3) == 3);

    klist_destroy(int_list);

    return 0;
}

DEFINE_TEST(TEST_LIST_REMOVE_ALL)
{
    klist *int_list = klist_new();
    CHECK_TEST(klist_size(int_list) == 0);

    klist_add(int_list, 1);
    klist_add(int_list, 2);
    klist_add(int_list, 3);

    CHECK_TEST(klist_size(int_list) == 3);
    klist_remove_all(int_list);
    CHECK_TEST(klist_size(int_list) == 0);

    klist_destroy(int_list);

    return 0;
}

DEFINE_TEST(TEST_LIST_REMOVE)
{
    klist *int_list = klist_new();
    CHECK_TEST(klist_size(int_list) == 0);

    klist_add(int_list, 1);
    klist_add(int_list, 2);
    klist_add(int_list, 3);
    CHECK_TEST(klist_size(int_list) == 3);

    klist_remove(int_list, 2);
    CHECK_TEST(klist_size(int_list) == 2);
    CHECK_TEST(klist_get_first(int, int_list) == 1);
    CHECK_TEST(klist_get_last(int, int_list) == 3);

    klist_destroy(int_list);

    return 0;
}

DEFINE_TEST(TEST_LIST_REMOVE_AT)
{
    klist *int_list = klist_new();
    CHECK_TEST(klist_size(int_list) == 0);

    klist_add(int_list, 1);
    klist_add(int_list, 2);
    klist_add(int_list, 3);
    CHECK_TEST(klist_size(int_list) == 3);

    klist_remove_at(int_list, 1);
    CHECK_TEST(klist_size(int_list) == 2);
    CHECK_TEST(klist_get_first(int, int_list) == 1);
    CHECK_TEST(klist_get_last(int, int_list) == 3);

    klist_destroy(int_list);

    return 0;
}

DEFINE_TEST(TEST_LIST_REMOVE_LAST)
{
    klist *int_list = klist_new();
    CHECK_TEST(klist_size(int_list) == 0);

    klist_add(int_list, 1);
    klist_add(int_list, 2);
    klist_add(int_list, 3);
    CHECK_TEST(klist_size(int_list) == 3);

    klist_remove_last(int_list);
    CHECK_TEST(klist_size(int_list) == 2);
    CHECK_TEST(klist_get_first(int, int_list) == 1);
    CHECK_TEST(klist_get_last(int, int_list) == 2);

    klist_destroy(int_list);

    return 0;
}

DEFINE_TEST(TEST_LIST_REMOVE_FIRST)
{
    klist *int_list = klist_new();
    CHECK_TEST(klist_size(int_list) == 0);

    klist_add(int_list, 1);
    klist_add(int_list, 2);
    klist_add(int_list, 3);
    CHECK_TEST(klist_size(int_list) == 3);

    klist_remove_first(int_list);
    CHECK_TEST(klist_size(int_list) == 2);
    CHECK_TEST(klist_get_first(int, int_list) == 2);
    CHECK_TEST(klist_get_last(int, int_list) == 3);

    klist_destroy(int_list);

    return 0;
}
