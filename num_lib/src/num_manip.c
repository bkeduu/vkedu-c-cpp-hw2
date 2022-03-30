#include <string.h>
#include <math.h>

#include "num_manip.h"
#include "num_utils.h"

int get_max(size_t* arr, size_t size) {

    if(arr == NULL || size == 0) {
        return ERR_NULL;
    }

    int max = arr[0];

    for(size_t i = 0; i < size; ++i) {
        if(max < arr[i]) {
            max = arr[i];
        }
    }
    return max;
}

long get_sum(size_t* arr, size_t size) {

    if(arr == NULL) {
        return ERR_NULL;
    }

    int sum = 0;

    for(size_t i = 0; i < size; ++i) {
        sum += arr[i];
    }
    return sum;
}

size_t get_dig_cnt(int num) {
    return ceil(log10((num > 0) ? num : -num));
}

int comp(const int* a, const int* b) {
    return *a - *b;
}

void m_sort(array_t array) {
    qsort(array.arr, array.size, sizeof(int), (int(*)(const void*, const void*)) comp);
}
