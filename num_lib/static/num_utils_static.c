#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "num_utils.h"
#include "num_manip.h"

errors_t handle_files(string_t* file_names, size_t files_count, size_t proc_count_ignored) {
    errors_t code = proc_count_ignored;  // to suppress warning that last parameter aren't used, this value will not be used

    if(file_names == NULL || files_count == 0) {
        return ERR_NULL;
    }

    array_t* arrays = malloc(sizeof(array_t) * files_count);

    for(size_t i = 0; i < files_count; ++i) {
        arrays[i].arr = NULL;
        arrays[i].size = 0;
        arrays[i].vec_name = NULL;
    }

    for(size_t i = 0; i < files_count; ++i) {
        code = get_vector(&arrays[i], file_names[i]);

        if (code != 0) {
            free_arrays(&arrays, files_count);
            return code;
        }
    }

    for(size_t i = 0; i < files_count; ++i) {
        sort(arrays[i]);
    }

    if(code != 0) {
        free_arrays(&arrays, files_count);
        return code;
    }

    for(size_t i = 0; i < files_count; ++i) {
        code = print_info(arrays[i], 0);

        if(code != 0) {
            free_arrays(&arrays, files_count);
            return code;
        }
    }

    free_arrays(&arrays, files_count);

    return 0;
}

errors_t get_digits_count(array_t* array, size_t proc_count, size_t** digits_count) {
    if(array == NULL) {
        return ERR_NULL;
    }

    *digits_count = malloc(sizeof(size_t) * DIGITS_COUNT);

    if(*digits_count == NULL) {
        return ERR_MALLOC;
    }

    for (size_t k = 0; k < DIGITS_COUNT; ++k) {
        (*digits_count)[k] = 0;
    }

    for(size_t i = 0; i < array->size; ++i) {

        int curr_value = array->arr[i];

        while (curr_value > 0) {
            (*digits_count)[curr_value % DIGITS_COUNT]++;
            curr_value /= 10;
        }

    }

    return 0;
}


