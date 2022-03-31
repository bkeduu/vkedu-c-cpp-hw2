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
    string_t argv[] = {(string_t)"./a.out", (string_t)"file1.txt", (string_t)"file2.txt", (string_t)"-j", (string_t)"0"};
    int argc = 5;
    size_t proc_count = 0;
    size_t files_count = 0;
    string_t* file_names = NULL;

    EXPECT_EQ(ERR_PROC_PARAM, get_params(argv, argc, &file_names, &files_count, &proc_count));
}

TEST(test_NULL, handle_files_tests) {
    EXPECT_EQ(ERR_NULL, handle_files(NULL, 0, 0));
}

TEST(test_many_proc, handle_files_tests) {
    string_t files[] = {(string_t)"file1.txt", (string_t)"file2.txt"};
    EXPECT_EQ(ERR_MORE_PROC, handle_files(files, 2, 1000000));
}

TEST(test_NULL, open_file_tests) {
    EXPECT_EQ(ERR_NULL, open_file(NULL, NULL));
}

TEST(test_bad_file, open_file_tests) {
    file_t f;
    EXPECT_EQ(ERR_FOPEN, open_file((string_t)"some_file_name", &f));
}

TEST(test_normal, open_file_tests) {
    file_t f;
    f = fopen("some_file_name", "w+");
    fclose(f);
    EXPECT_EQ(0, open_file((string_t)"some_file_name", &f));
    fclose(f);
}

