#include <gtest/gtest.h>
#include <string>
#include <iostream>

extern "C" {
#include "num_utils.h"
#include "num_manip.h"
}

TEST(test_NULL, mp_get_params_tests) {
    EXPECT_EQ(ERR_NULL, mp_get_params(NULL, 0, NULL, NULL, NULL));
}

TEST(test_bad_proc, mp_get_params_tests) {
    string_t argv[] = {(string_t)"./a.out", (string_t)"file1.txt", (string_t)"file2.txt", (string_t)"-j", (string_t)"0"};
    int argc = 5;
    size_t proc_count = 0;
    size_t files_count = 0;
    string_t* file_names = NULL;

    EXPECT_EQ(ERR_PROC_PARAM, mp_get_params(argv, argc, &file_names, &files_count, &proc_count));
}

TEST(test_NULL, handle_files_tests) {
    EXPECT_EQ(ERR_NULL, handle_files(NULL, 0));
}
