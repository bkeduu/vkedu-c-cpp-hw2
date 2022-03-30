#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/wait.h>

#include "num_utils.h"
#include "num_manip.h"

error_t get_params(string_t argv[], size_t argc, string_t** file_names, size_t* files_cnt, size_t* proc_cnt) {
    if(files_cnt == NULL || proc_cnt == NULL || argv == NULL || file_names == NULL) {
        return ERR_NULL;
    }

    int opt = 0;

    while((opt = getopt((int)argc, argv, "-:j:")) != -1) {
        if(opt == 'j') {
            size_t proc_arg = strtol(optarg, NULL, 10);
            if (proc_arg > 0) {
                *proc_cnt = proc_arg;
            }
            else {
                for(size_t i = 0; i < *files_cnt; ++i) {
                    free((*file_names)[i]);
                }
                free(*file_names);

                return ERR_PROC_PARAM;
            }
        }
        else if(opt == 1) {
            if(*file_names == NULL) {
                *file_names = malloc(sizeof(string_t*));

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

            strcpy((*file_names)[*files_cnt - 1], optarg);
        }
        else {
            for(size_t i = 0; i < *files_cnt; ++i) {
                free((*file_names)[i]);
            }
            free(*file_names);

            return ERR_UNKNWN_PARAM;
        }
    }

    if(*proc_cnt == 0) {
        *proc_cnt = 1;
    }

    if(*files_cnt == 0) {
        for(size_t i = 0; i < *files_cnt; ++i) {
            free((*file_names)[i]);
        }
        free(*file_names);
        return ERR_MALLOC;
    }

    return 0;
}

error_t start(string_t file_names[], size_t files_count, size_t proc_count) {
    if(file_names == NULL || files_count == 0 || proc_count == 0) {
        return ERR_NULL;
    }

    if(proc_count > sysconf(_SC_NPROCESSORS_ONLN)) {
        return ERR_MORE_PROC;
    }

    pid_t* pids = malloc(sizeof(pid_t) * proc_count);

    for(size_t i = 0; i < proc_count; ++i) {

        pids[i] = fork();

        if(pids[i] < 0) {
            free(pids);
            return ERR_PROC;
        }
        else if(pids[i] == 0) {
            for(size_t j = i; j < files_count; j += proc_count) {
                error_t code = 0;
                array_t* arr = malloc(sizeof(array_t));

                code = get_array(arr, file_names[j]);

                if (code != 0) {
                    for(size_t k = 0; k < i; ++k) {
                        waitpid(pids[k], NULL, 0);
                    }

                    free(arr);
                    free(pids);
                    return code;
                }

                m_sort(arr);

                print_result(arr);

                free(arr->arr);
                free(arr);
            }

            free(pids);
            return 0;
        }
    }

    for(size_t i = 0; i < proc_count; ++i) {
        waitpid(pids[i], NULL, 0);
    }

    free(pids);
    return 0;
}

error_t open_file(string_t file_name, file_t* file) {
    if(file == NULL) {
        return ERR_NULL;
    }

    *file = fopen(file_name, "r");

    if(*file == NULL) {
        return ERR_FOPEN;
    }

    return 0;
}

error_t get_array(array_t* result, string_t file_name) {
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

error_t print_result(array_t* array) {
    error_t code = print_median(array);

    if(code != 0) {
        return code;
    }

    code = draw_hist(array);

    return code;
}

error_t print_median(array_t* array) {
    if(array == NULL) {
        return ERR_NULL;
    }

    printf("Vector: %s, median: %d\n", array->vec_name, array->arr[array->size / 2]);

    return 0;
}

error_t draw_hist(array_t* array) {
    if(array == NULL) {
        return ERR_NULL;
    }

    printf("\nDigits distribution histogram for \"%s\" vector:\n", array->vec_name);

    size_t* digits_count = malloc(sizeof(size_t) * 10);

    if(digits_count == NULL) {
        return ERR_MALLOC;
    }

    for (size_t k = 0; k < 10; ++k) {
        digits_count[k] = 0;
    }

    for (size_t k = 0; k < array->size; ++k) {
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

void print_message(error_t code) {
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
        default:
            break;
    }
}

