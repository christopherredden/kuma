#include "list.h"

DEFINE_TEST(TEST_LIST)
{
    klist *int_list = klist_new();
    CHECK_TEST(klist_size(int_list) == 0);

    klist_add_first(int_list, (void*)1);
    CHECK_TEST(klist_size(int_list) == 1);
    CHECK_TEST((int)klist_get_at(int_list, 0) == 1);

    klist_add_first(int_list, (void*)2);
    CHECK_TEST((int)klist_size(int_list) == 2);
    CHECK_TEST((int)klist_get_at(int_list, 0) == 2);
    CHECK_TEST((int)klist_get_at(int_list, 1) == 1);

    klist_add_first(int_list, (void*)3);
    CHECK_TEST(klist_size(int_list) == 3);
    CHECK_TEST((int)klist_get_at(int_list, 0) == 3);
    CHECK_TEST((int)klist_get_at(int_list, 1) == 2);
    CHECK_TEST((int)klist_get_at(int_list, 2) == 1);

    return 0;
}
