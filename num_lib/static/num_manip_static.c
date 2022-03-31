#include "num_manip.h"
#include "num_utils.h"

void sort(array_t array) {
    qsort(array.arr, array.size, sizeof(int), (int(*)(const void*, const void*)) comp);
}
