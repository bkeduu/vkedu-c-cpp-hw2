#include <gtest/gtest.h>
#include <string>
#include <iostream>

extern "C" {
#include "num_utils.h"
#include "num_manip.h"
}

TEST(test_NULL, get_params_tests) {
    EXPECT_EQ(ERR_NULL, get_params(NULL, 0, NULL, NULL, NULL));
}

TEST(test_bad_proc, get_params_tests) {
    string_t argv[] = { (string_t)"./a.out", (string_t)"file1.txt", (string_t)"file2.txt", (string_t)"-j", (string_t)"0" };
    int argc = 5;
    size_t proc_count = 0;
    size_t files_count = 0;
    string_t* file_names = NULL;

    EXPECT_EQ(ERR_PROC_PARAM, get_params(argv, argc, &file_names, &files_count, &proc_count));
}

TEST(test_getopt_bad_param, get_params_tests) {
    string_t argv[] = { (string_t)"./tests/num_tests_shared", (string_t)"file1.txt", (string_t)"file2.txt", (string_t)"-j", (string_t)"0" };
    int argc = 5;
    size_t proc_count = 0;
    size_t files_count = 0;
    string_t* file_names = NULL;

    EXPECT_EQ(ERR_NO_FILES, get_params(argv, argc, &file_names, &files_count, &proc_count));
}

TEST(test_print_info_NULL, print_info_tests) {
    array_t arr = {0, NULL, NULL};
    print_info(arr, 0);
}

TEST(test_print_info_normal, print_info_tests) {
    int array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
    array_t arr = {sizeof(arr) / sizeof(int), array, (string_t)"test"};
    print_info(arr, 1);
}

TEST(test_get_digits_count_NULL, get_digits_count_tests) {
    EXPECT_EQ(ERR_NULL, get_digits_count(NULL, 0, NULL));
}

TEST(test_handle_files_NULL, handle_files_tests) {
    EXPECT_EQ(ERR_NULL, handle_files(NULL, 0, 0));
}

TEST(test_handle_files_normal, handle_files_tests) {
    string_t file_names[] = {(string_t)"../vec_0.txt"};
    EXPECT_EQ(0, handle_files(file_names, 1, 1));
}

TEST(test_print_message, print_message_tests) {
    print_message(ERR_MALLOC);
    print_message(ERR_NULL);
    print_message(ERR_FOPEN);
    print_message(ERR_MORE_PROC);
    print_message(ERR_UNKNWN_PARAM);
    print_message(ERR_PROC_PARAM);
    print_message(ERR_PROC);
    print_message(ERR_NO_FILES);
    print_message(NERROR_PROC_EXIT);
}

TEST(test_get_vector_NULL, get_vector_tests) {
    EXPECT_EQ(ERR_NULL, get_vector(NULL, NULL));
}

TEST(test_get_vector_normal, get_vector_tests) {
    array_t res;
    string_t file_names[] = {(string_t)"../vec_0.txt"};
    EXPECT_EQ(0, get_vector(&res, file_names[0]));
    free(res.arr);
}

TEST(test_get_max_NULL, get_max_tests) {
    EXPECT_EQ(ERR_NULL, get_max(NULL, 0));
}

TEST(test_get_sum_NULL, get_sum_tests) {
    EXPECT_EQ(ERR_NULL, get_sum(NULL, 0));
}

TEST(test_open_file_NULL, open_file_tests) {
    EXPECT_EQ(ERR_NULL, open_file(NULL, NULL));
}

TEST(test_sort, sort_tests) {
    array_t res;
    string_t file_names[] = {(string_t)"../vec_0.txt"};
    EXPECT_EQ(0, get_vector(&res, file_names[0]));
    sort(res);
    free(res.arr);
}
