#pragma once

#include <stdio.h>

typedef FILE* file_t;
typedef char* string_t;

typedef struct array_t {
    size_t size;
    int* arr;
    string_t vec_name;
} array_t;

typedef enum errors_t {
    ERR_MALLOC = -1,
    ERR_NULL = -2,
    ERR_FOPEN = -3,
    ERR_MORE_PROC = -4,
    ERR_UNKNWN_PARAM= -5,
    ERR_PROC_PARAM = -6,
    ERR_PROC = -7,
    ERR_NO_FILES = -8,
    NERROR_PROC_EXIT = -9
} errors_t;


errors_t open_file(string_t, file_t*);
errors_t get_array(array_t*, string_t);

void print_message(errors_t);

errors_t mp_get_params(string_t[], size_t, string_t**, size_t*, size_t*);
errors_t mp_print_info(array_t*, size_t);

errors_t get_params(string_t [], size_t, string_t**, size_t*);
errors_t handle_files(string_t [], size_t);
errors_t print_info(array_t*);
