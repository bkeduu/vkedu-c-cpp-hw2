#pragma once

#include <stdio.h>

typedef FILE* file_t;
typedef char* string_t;

typedef struct array_t {
    size_t size;
    int* arr;
    string_t vec_name;
} array_t;

typedef enum error_t {
    ERR_MALLOC = -1,
    ERR_NULL = -2,
    ERR_FOPEN = -3,
    ERR_MORE_PROC = -4,
    ERR_UNKNWN_PARAM= -5,
    ERR_PROC_PARAM = -6,
    ERR_PROC = -7,
    ERR_NO_FILES = -8,
} error_t;

error_t get_params(string_t argv[], size_t argc, string_t** file_names, size_t* files_cnt, size_t* proc_cnt);
error_t start(string_t [], size_t, size_t);

error_t open_file(string_t, file_t*);
error_t get_array(array_t*, string_t);

error_t print_result(array_t*);
error_t print_median(array_t*);
error_t draw_hist(array_t*);

void print_message(error_t);
