#include <stdio.h>
#include <stdlib.h>

#include "num_utils.h"

errors_t open_file(string_t file_name, file_t* file) {
    if(file == NULL) {
        return ERR_NULL;
    }

    *file = fopen(file_name, "r");

    if(*file == NULL) {
        return ERR_FOPEN;
    }

    return 0;
}

errors_t get_array(array_t* result, string_t file_name) {
    if(result == NULL || file_name == NULL) {
        return ERR_NULL;
    }

    file_t file = NULL;
    short err_code = open_file(file_name, &file);

    if(err_code != 0) {
        return err_code;
    }

    size_t buff_size = 1;
    size_t real_size = 0;
    int* arr = malloc(sizeof(int));

    if(arr == NULL) {
        fclose(file);
        return ERR_MALLOC;
    }

    int value = 0;

    while(!feof(file) && (fscanf(file, "%d", &value) == 1)) {

        if(real_size == buff_size) {
            int* tmp = realloc(arr, sizeof(int) * buff_size * 2);

            if(tmp == NULL) {
                free(arr);
                fclose(file);
                return ERR_MALLOC;
            }

            arr = tmp;
            buff_size *= 2;
        }

        arr[real_size++] = value;
    }

    int* res_arr = realloc(arr, sizeof(int) * real_size);

    if(res_arr == NULL) {
        free(arr);
        fclose(file);
        return ERR_MALLOC;
    }

    result->size = real_size - 1;
    result->arr = res_arr;
    result->vec_name = file_name;

    fclose(file);

    return 0;
}

void print_message(errors_t code) {
    switch(code) {
        case ERR_MALLOC:
            printf("Malloc error in program, exiting...\n");
            return;
        case ERR_NULL:
            printf("NULL pointer given to function, exiting...\n");
            return;
        case ERR_FOPEN:
            printf("Unable to open file(s), exiting...\n");
            return;
        case ERR_MORE_PROC:
            printf("You\'re requiring more processes, that your system can handle, exiting...\n");
            return;
        case ERR_UNKNWN_PARAM:
            printf("Unknown parameter given, exiting...\n");
            return;
        case ERR_PROC_PARAM:
            printf("Parameter after -j should be a positive number, exiting...\n");
            return;
        case ERR_NO_FILES:
            printf("No files given, exiting...\n");
            return;
        case ERR_PROC:
            printf("Error while creating child process, exiting...\n");
        default:
            break;
    }
}
