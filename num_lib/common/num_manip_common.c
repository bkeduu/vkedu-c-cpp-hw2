#include <math.h>

#include "num_manip.h"
#include "num_utils.h"

int get_max(const size_t* arr, size_t size) {
    if (arr == NULL || size == 0) {
        return ERR_NULL;
    }

    int max = (int)arr[0];

    for (size_t i = 0; i < size; ++i) {
        if (max < arr[i]) {
            max = (int)arr[i];
        }
    }
    return max;
}

long get_sum(const size_t* arr, size_t size) {
    if (arr == NULL) {
        return ERR_NULL;
    }

    int sum = 0;

    for (size_t i = 0; i < size; ++i) {
        sum += (int)arr[i];
    }
    return sum;
}

size_t get_dig_cnt(int num) {
    return ceil(log10((num > 0) ? num : -num));
}

int comp(const int* a, const int* b) {
    return *a - *b;
}
