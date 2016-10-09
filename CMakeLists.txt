cmake_minimum_required(VERSION 3.6)
project(kuma)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(./)

set(SOURCE_FILES
    ast.c
    ast.h
    kuma.c
    lexer.c
    lexer.h
    list.c
    list.h
    parser.c
    parser.h
    table.c
    table.h)

set(TEST_FILES
    test/list_test.h
    test/test.c
    test/test.h)

add_executable(kuma ${SOURCE_FILES})
add_executable(kuma-test ${TEST_FILES})