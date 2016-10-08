#ifndef KUMA_TEST_H
#define KUMA_TEST_H

#include "stdlib.h"
#include "stdio.h"

#define DEFINE_TEST(name) \
    int kuma_test_##name()

#define CHECK_TEST(cond) \
    if(!(cond)) \
    { \
        printf("%s\n", #cond); \
        return 1; \
    }\

#define RUN_TEST(name) \
    { \
        int r = kuma_test_##name(); \
        if(r == 0) \
            printf("%s: Suceeded\n", #name); \
        else \
            printf("%s: Failed\n", #name); \
    } \

#endif
