#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "num_utils.h"
#include "num_manip.h"


errors_t get_params(string_t argv[], size_t argc, string_t** file_names, size_t* files_cnt) {
    if(files_cnt == NULL || argv == NULL || file_names == NULL) {
        return ERR_NULL;
    }

    int opt = 0;

    while((opt = getopt((int)argc, argv, "-:")) != -1) {
        if(opt == 1) {
            if(*file_names == NULL) {
                *file_names = malloc(sizeof(string_t));

                if(*file_names == NULL) {
                    return ERR_MALLOC;
                }

                ++(*files_cnt);
            }
            else {
                string_t* tmp = realloc(*file_names, sizeof(string_t) * (*files_cnt + 1));

                if(tmp == NULL) {
                    for(size_t i = 0; i < *files_cnt; ++i) {
                        free((*file_names)[i]);
                    }
                    free(*file_names);
                    return ERR_MALLOC;
                }

                ++(*files_cnt);
                *file_names = tmp;
            }

            (*file_names)[*files_cnt - 1] = malloc(sizeof(char) * (strlen(optarg) + 1));

            if((*file_names)[*files_cnt - 1] == NULL) {
                for(size_t i = 0; i < *files_cnt - 1; ++i) {
                    free((*file_names)[i]);
                }
                free(*file_names);
            }

            strncpy((*file_names)[*files_cnt - 1], optarg, strlen(optarg) + 1);
        }
        else {
            for(size_t i = 0; i < *files_cnt; ++i) {
                free((*file_names)[i]);
            }
            free(*file_names);

            return ERR_UNKNWN_PARAM;
        }
    }

    if(*files_cnt == 0) {
        for(size_t i = 0; i < *files_cnt; ++i) {
            free((*file_names)[i]);
        }
        free(*file_names);
        return ERR_NO_FILES;
    }

    return 0;
}

errors_t handle_files(string_t argv[], size_t argc) {

    string_t* file_names = NULL;
    size_t files_count = 0;
    errors_t code = 0;

    code = get_params(argv, argc, &file_names, &files_count);

    if(file_names == NULL || files_count == 0) {
        return ERR_NULL;
    }

    array_t* arrays = malloc(sizeof(array_t) * files_count);


    for(size_t i = 0; i < files_count; ++i) {
        code = 0;

        code = get_array(&arrays[i], file_names[i]);

        if (code != 0) {
            for(size_t j = 0; j < files_count; ++j){
                free(arrays[j].arr);
            }
            free(arrays);

            for(size_t j = 0; j < files_count; ++j) {
                free(file_names[j]);
            }
            free(file_names);

            return code;
        }
    }

    for(size_t i = 0; i < files_count; ++i) {

        code = sort(arrays[i]);

        if (code != 0) {
            for (size_t j = 0; j < files_count; ++j) {
                free(arrays[j].arr);
            }
            free(arrays);

            return code;
        }
    }

    if(code != 0) {
        for(size_t j = 0; j < files_count; ++j){
            free(arrays[j].arr);
        }
        free(arrays);

        for(size_t j = 0; j < files_count; ++j) {
            free(file_names[j]);
        }
        free(file_names);

        return code;
    }

    for(size_t i = 0; i < files_count; ++i) {
        code = print_info(&arrays[i]);

        if(code != 0) {
            for(size_t j = 0; j < files_count; ++j){
                free(arrays[j].arr);
            }
            free(arrays);

            for(size_t j = 0; j < files_count; ++j) {
                free(file_names[j]);
            }
            free(file_names);
            return code;
        }
    }

    for(size_t i = 0; i < files_count; ++i) {
        free(arrays[i].arr);
    }
    free(arrays);

    for(size_t i = 0; i < files_count; ++i) {
        free(file_names[i]);
    }
    free(file_names);

    return 0;
}

errors_t print_info(array_t* array) {
    if(array == NULL) {
        return ERR_NULL;
    }

    printf("\nVector: %s, median: %d\n", array->vec_name, array->arr[array->size / 2]);
    printf("Digits distribution histogram for \"%s\" vector:\n\n", array->vec_name);

    size_t* digits_count = malloc(sizeof(size_t) * 10);

    if(digits_count == NULL) {
        return ERR_MALLOC;
    }

    for (size_t k = 0; k < 10; ++k) {
        digits_count[k] = 0;
    }

    for(size_t k = 0; k < array->size; ++k) {

        int curr_value = array->arr[k];

        while (curr_value > 0) {
            digits_count[curr_value % 10]++;
            curr_value /= 10;
        }

    }

    long dig_cnt_sum = get_sum(digits_count, 10);
    double mid = (double)dig_cnt_sum / 10;
    int max_width = get_dig_cnt(get_max(digits_count, 10));


    for (size_t k = 0; k < 10; ++k) {
        printf("%ld: %*ld, %5.2lf%c | ", k, max_width, digits_count[k], (double)digits_count[k] / (double)dig_cnt_sum * 100, '%');

        for (size_t j = 0; j < (double)digits_count[k] / mid * 50; ++j) {
            printf("*");
        }

        printf("\n");
    }

    free(digits_count);

    return 0;
}
