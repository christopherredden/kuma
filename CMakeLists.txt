cmake_minimum_required(VERSION 3.6)
project(kuma)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(./)

set(SOURCE_FILES
    ast.c
    ast.h
    lexer.c
    lexer.h
    list.c
    list.h
    parser.c
    parser.h
    table.c
    table.h)

set(KUMAC_SOURCE_FILES
    kumac.c)

set(TEST_FILES
    test/list_test.h
    test/test.c
    test/test.h)

add_library(kuma ${SOURCE_FILES})
add_executable(kuma-test ${TEST_FILES})
add_executable(kumac ${KUMAC_SOURCE_FILES})
target_link_libraries (kumac LINK_PUBLIC kuma)
target_link_libraries (kuma-test LINK_PUBLIC kuma)
