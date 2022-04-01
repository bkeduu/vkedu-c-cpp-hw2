#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "num_utils.h"
#include "num_manip.h"

errors_t get_params(string_t argv[], size_t argc, string_t** file_names,
                    size_t* files_count, size_t* proc_cnt) {
    if (files_count == NULL || argv == NULL || file_names == NULL) {
        return ERR_NULL;
    }

    int opt;

    while ((opt = getopt((int)argc, argv, "-:j:")) != -1) {
        if (opt == 'j' && proc_cnt != NULL) {
            size_t proc_arg = strtoul(optarg, NULL, 10);
            if (proc_arg > 0) {
                *proc_cnt = proc_arg;
            } else {
                free_fnames(file_names, *files_count);
                return ERR_PROC_PARAM;
            }
        } else if (opt == 1) {
            errors_t error_code = add_file(file_names, files_count, optarg);

            if (error_code != 0) {
                free_fnames(file_names, *files_count);
                return error_code;
            }
        } else {
            free_fnames(file_names, *files_count);
            return ERR_UNKNWN_PARAM;
        }
    }

    if (*files_count == 0) {
        free_fnames(file_names, *files_count);
        return ERR_NO_FILES;
    }

    return 0;
}

errors_t open_file(string_t file_name, file_t* file) {
    if (file == NULL) {
        return ERR_NULL;
    }

    *file = fopen(file_name, "r");

    if (*file == NULL) {
        return ERR_FOPEN;
    }

    return 0;
}

errors_t get_vector(array_t* result, string_t file_name) {
    if (result == NULL || file_name == NULL) {
        return ERR_NULL;
    }

    file_t file = NULL;
    short err_code = open_file(file_name, &file);

    if (err_code != 0) {
        return err_code;
    }

    size_t buff_size = 1;
    size_t real_size = 0;
    int* arr = malloc(sizeof(int));

    if (arr == NULL) {
        fclose(file);
        return ERR_MALLOC;
    }

    int value = 0;

    while (!feof(file) && (fscanf(file, "%d", &value) == 1)) {
        if (real_size == buff_size) {
            int* tmp = realloc(arr, sizeof(int) * buff_size * 2);

            if (tmp == NULL) {
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

    if (res_arr == NULL) {
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
    switch (code) {
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
            printf("Error when creating child process, exiting...\n");
            break;
        default:
            break;
    }
}

void free_arrays(array_t** arrays, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        free((*arrays)[i].arr);
    }
    free(*arrays);
}

void free_fnames(string_t** arrays, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        free((*arrays)[i]);
    }
    free(*arrays);
}

errors_t add_file(string_t** file_names, size_t* files_count, string_t file_name) {
    if (*file_names == NULL) {
        *file_names = malloc(sizeof(string_t));

        if (*file_names == NULL) {
            return ERR_MALLOC;
        }

        ++(*files_count);
    } else {
        string_t* tmp = realloc(*file_names, sizeof(string_t) * (*files_count + 1));

        if (tmp == NULL) {
            free_fnames(file_names, *files_count);
            return ERR_MALLOC;
        }

        ++(*files_count);
        *file_names = tmp;
    }

    (*file_names)[*files_count - 1] = malloc(sizeof(char) * (strlen(file_name) + 1));

    if ((*file_names)[*files_count - 1] == NULL) {
        free_fnames(file_names, *files_count);
        return ERR_MALLOC;
    }

    strncpy((*file_names)[*files_count - 1], optarg, strlen(file_name) + 1);
    return 0;
}

errors_t print_info(array_t array, size_t proc_count) {
    if (array.arr == NULL || array.size == 0) {
        return ERR_NULL;
    }

    size_t* digits_count = NULL;

    errors_t error_code = get_digits_count(&array, proc_count, &digits_count);

    if (error_code != 0) {
        return error_code;
    }

    long dig_cnt_sum = get_sum(digits_count, DIGITS_COUNT);
    double mid = (double)dig_cnt_sum / DIGITS_COUNT;
    int max_width = (int)get_dig_cnt(get_max(digits_count, DIGITS_COUNT));

    printf("\nVector: %s, median: %d\n", array.vec_name, array.arr[array.size / 2]);
    printf("Digits distribution histogram for \"%s\" vector:\n\n", array.vec_name);

    for (size_t k = 0; k < DIGITS_COUNT; ++k) {
        printf("%ld: %*ld, %5.2lf%c | ", k, max_width, digits_count[k],
               (double)digits_count[k] / (double)dig_cnt_sum * 100, '%');

        for (size_t j = 0; j < (size_t)((double)digits_count[k] / mid * 50); ++j) {
            printf("*");
        }

        printf("\n");
    }

    free(digits_count);

    return 0;
}
