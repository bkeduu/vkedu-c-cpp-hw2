#pragma once

#include <stdio.h>

typedef struct DArray {
    size_t size;
    int* arr;
} DArray;

typedef enum errors {
    ERR_MALLOC = -1,
    ERR_NULL = -2,
    ERR_FOPEN = -3,
    ERR_MORE_PROC = -4,
    ERR_UNKNWN_PARAM= -5,
    ERR_PROC_PARAM = -6,
    ERR_PROC = -7
} errors;

typedef FILE* file;
typedef char* string;

short get_params(string argv[], size_t argc, string** file_names, size_t* files_cnt, size_t* proc_cnt);
short start(string [], size_t);

short open_files(string*, size_t, file*);
short get_arrays(file*, size_t, DArray*, size_t);
short get_arr(file, DArray*);

short print_medians(DArray*, size_t);
short draw_hist(DArray*, size_t);
