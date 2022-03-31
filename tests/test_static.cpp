#include <gtest/gtest.h>
#include <string>
#include <iostream>

extern "C" {
#include "num_utils.h"
#include "num_manip.h"
}

TEST(test_NULL, get_params_tests) {
    EXPECT_EQ(ERR_NULL, get_params(NULL, 0, NULL, NULL));
}

TEST(test_NULL, handle_files_tests) {
    EXPECT_EQ(ERR_NULL, handle_files(NULL, 0));
}
