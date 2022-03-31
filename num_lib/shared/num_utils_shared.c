#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>

#include <sys/mman.h>
#include <sys/wait.h>

#include "num_utils.h"
#include "num_manip.h"

errors_t mp_get_params(string_t argv[], size_t argc, string_t** file_names, size_t* files_cnt, size_t* proc_cnt) {
    if(files_cnt == NULL || proc_cnt == NULL || argv == NULL || file_names == NULL) {
        return ERR_NULL;
    }

    int opt;

    while((opt = getopt((int)argc, argv, "-:j:")) != -1) {
        if(opt == 'j') {
            size_t proc_arg = strtoul(optarg, NULL, 10);
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
    size_t proc_count = 0;
    errors_t code = mp_get_params(argv, argc, &file_names, &files_count, &proc_count);


    if(code != 0) {
        return code;
    }

    if(file_names == NULL || files_count == 0) {
        return ERR_NULL;
    }

    if(proc_count > sysconf(_SC_NPROCESSORS_ONLN)) {
        return ERR_MORE_PROC;
    }

    if(proc_count == 0) {
        proc_count = sysconf(_SC_NPROCESSORS_ONLN);
    }

    array_t* arrays = malloc(sizeof(array_t) * files_count);


    for(size_t i = 0; i < files_count; ++i) {

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

    code = mp_sort(&arrays, files_count, proc_count);

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
        code = mp_print_info(&arrays[i], proc_count);

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

errors_t mp_print_info(array_t* array, size_t proc_count) {
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

    size_t* sh_digits_count = mmap(NULL, sizeof(size_t) * 10, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    memcpy(sh_digits_count, digits_count, sizeof(size_t) * 10);

    sem_t* semaphore = malloc(sizeof(sem_t));

    if(semaphore == NULL) {
        return ERR_MALLOC;
    }

    sem_init(semaphore, 1, 1);
    sem_t* sh_semaphore = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    memcpy(sh_semaphore, semaphore, sizeof(sem_t));

    pid_t *pids = malloc(sizeof(pid_t) * proc_count);

    if(pids == NULL) {
        return ERR_MALLOC;
    }

    for (size_t i = 0; i < proc_count; ++i) {

        pids[i] = fork();

        if (pids[i] < 0) {
            free(pids);
            return ERR_PROC;
        } else if (pids[i] == 0) {

            for(size_t k = i; k < array->size; k += proc_count) {

                sem_wait(sh_semaphore);

                int curr_value = array->arr[k];

                while (curr_value > 0) {
                    sh_digits_count[curr_value % 10]++;
                    curr_value /= 10;
                }

                sem_post(sh_semaphore);
            }

            sem_destroy(semaphore);
            free(digits_count);
            free(semaphore);
            free(pids);
            return NERROR_PROC_EXIT;
        }
    }

    for (size_t j = 0; j < proc_count; ++j) {
        waitpid(pids[j], NULL, 0);
    }

    sem_destroy(semaphore);
    free(semaphore);
    free(pids);

    memcpy(digits_count, sh_digits_count, sizeof(size_t) * 10);
    munmap(sh_digits_count, sizeof(size_t) * 10);
    munmap(sh_semaphore, sizeof(sem_t));

    long dig_cnt_sum = get_sum(digits_count, 10);
    double mid = (double)dig_cnt_sum / 10;
    int max_width = (int)get_dig_cnt(get_max(digits_count, 10));


    for (size_t k = 0; k < 10; ++k) {
        printf("%ld: %*ld, %5.2lf%c | ", k, max_width, digits_count[k], (double)digits_count[k] / (double)dig_cnt_sum * 100, '%');

        for (size_t j = 0; j < (size_t)((double)digits_count[k] / mid * 50); ++j) {
            printf("*");
        }

        printf("\n");
    }

    free(digits_count);

    return 0;
}
