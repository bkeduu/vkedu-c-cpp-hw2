#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/wait.h>

#include "num_utils.h"
#include "num_manip.h"

short get_params(string argv[], size_t argc, string** file_names, size_t* files_cnt, size_t* proc_cnt) {
    if(files_cnt == NULL || proc_cnt == NULL || argv == NULL || file_names == NULL) {
        return ERR_NULL;
    }

    int opt;
    *proc_cnt = sysconf(_SC_NPROCESSORS_ONLN);

    while((opt = getopt((int)argc, argv, "-:j:")) != -1) {

        if(opt == 'j') {
            size_t proc_arg = strtoul(optarg, NULL, 10);
            if (proc_arg != 0) {
                if (proc_arg > *proc_cnt) {

                    for(size_t i = 0; i < *files_cnt; ++i) {
                        free((*file_names)[i]);
                    }
                    free(*file_names);

                    return ERR_MORE_PROC;
                }
                else {
                    *proc_cnt = proc_arg;
                }
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
                *file_names = malloc(sizeof(string*));

                if(*file_names == NULL)
                    return ERR_MALLOC;

                ++(*files_cnt);
            }
            else {
                string* tmp = realloc(*file_names, sizeof(string) * (*files_cnt + 1));

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

    return 0;
}

short start(string argv[], size_t argc) {

    short err_code;

    string* file_names = NULL;
    size_t files_count = 0;
    size_t proc_cnt = 0;

    err_code = get_params(argv, argc, &file_names, &files_count, &proc_cnt);

    if(err_code != 0) {
        return err_code;
    }

    file* files = malloc(sizeof(file) * files_count);
    err_code = open_files(file_names, files_count, files);

    for(size_t i = 0; i < files_count; ++i) {
        free(file_names[i]);
    }
    free(file_names);

    if(err_code != 0) {
        return err_code;
    }

    printf("Using %ld processes for %ld file(s) handling.\n", proc_cnt, files_count);

    DArray* arrays = malloc(sizeof(DArray) * files_count);
    err_code = get_arrays(files, files_count, arrays, proc_cnt);

    printf("\narrays[0].size: %ld\n", arrays[0].size);

    if(err_code != 0) {
        for(size_t i = 0; i < files_count; ++i) {
            free(arrays[i].arr);
        }
        free(arrays);

        for(size_t i = 0; i < files_count; ++i) {
            fclose(files[i]);
        }
        free(files);

        for(size_t i = 0; i < files_count; ++i) {
            free(arrays[i].arr);
        }
        free(arrays);

        return err_code;
    }

    m_sort(arrays, files_count);

    print_medians(arrays, files_count);
    draw_hist(arrays, files_count);

    for(size_t i = 0; i < files_count; ++i) {
        fclose(files[i]);
    }
    free(files);

    for(size_t i = 0; i < files_count; ++i) {
        free(arrays[i].arr);
    }
    free(arrays);

    return 0;
}

short open_files(string* file_names, size_t count, file* files) {
    if(files == NULL) {
        return ERR_NULL;
    }

    for(size_t i = 0; i < count; ++i) {
        files[i] = fopen(file_names[i], "r");

        if(!files[i]) {
            for(size_t j = 0; j < i; ++j) {
                fclose(files[j]);
            }

            free(files);
            return ERR_FOPEN;
        }
    }

    return 0;
}

short get_arrays(file* files, size_t count, DArray* arrays, size_t proc_count) {
    if(files == NULL || arrays == NULL) {
        return ERR_NULL;
    }

    DArray* p_arrays = mmap(NULL, sizeof(DArray) * count, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    memcpy(p_arrays, arrays, sizeof(DArray) * count);

    pid_t* pids = malloc(sizeof(pid_t) * proc_count);

    for(size_t i = 0; i < proc_count; ++i) {
        pids[i] = fork();

        if(pids[i] < 0) {
            free(pids);
            return ERR_PROC;
        }
        else if(pids[i] == 0) {
            for(size_t j = i; j < count; j += proc_count) {

                printf("\n\n\nj: %ld\n\n\n", j);

                short code = get_arr(files[j], &p_arrays[j]);

                if (code != 0) {
                    return code;
                }
            }

            exit(0);
        }
    }

    for(size_t i = 0; i < proc_count; ++i) {
        wait(NULL);
    }

    munmap(p_arrays, sizeof(DArray) * count);

    free(pids);
    return 0;
}

short get_arr(file file, DArray* result) {
    if(file == NULL || result == NULL) {
        return ERR_NULL;
    }

    size_t buff_size = 1;
    size_t real_size = 0;
    int* arr = malloc(sizeof(int));

    if(arr == NULL) {
        return ERR_MALLOC;
    }

    int value;

    while(!feof(file) && (fscanf(file, "%d", &value) == 1)) {

        if(real_size == buff_size) {
            int* tmp = realloc(arr, sizeof(int) * buff_size * 2);

            if(tmp == NULL) {
                free(arr);
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
        return ERR_MALLOC;
    }

    result->size = real_size - 1;
    result->arr = res_arr;

    return 0;
}

short print_medians(DArray* arrays, size_t size) {

    if(arrays == NULL) {
        return ERR_NULL;
    }

    int* medians = malloc(sizeof(int) * size);

    if(medians == NULL) {
        return ERR_MALLOC;
    }

    for(size_t i = 0; i < size; ++i) {
        medians[i] = arrays[i].arr[arrays[i].size / 2];
    }

    for(size_t i = 0; i < size; ++i) {
        printf("Vector: %ld, median: %d\n", i + 1, medians[i]);
    }

    free(medians);
    return 0;
}

short draw_hist(DArray* arrays, size_t size) {

    if(arrays == NULL) {
        return ERR_NULL;
    }

    for(size_t i = 0; i < size; ++i) {

        printf("\nDigits distribution histogram for %ld", i + 1);

        switch (i) {
            case 0:
                printf("\'st vector:\n");
                break;
            case 1:
                printf("\'nd vector:\n");
                break;
            case 2:
                printf("\'rd vector:\n");
                break;
            default:
                printf("\'th vector:\n");
                break;
        }

        size_t* digits_count = malloc(sizeof(size_t) * 10);

        if(digits_count == NULL) {
            return ERR_MALLOC;
        }

        for (size_t k = 0; k < 10; ++k) {
            digits_count[k] = 0;
        }

        for (size_t k = 0; k < arrays[i].size; ++k) {

            int curr_value = arrays[i].arr[k];

            while (curr_value > 0) {
                digits_count[curr_value % 10]++;
                curr_value /= 10;
            }
        }

        long dig_cnt_sum = get_sum(digits_count, 10);
        double mid = (double)dig_cnt_sum / 10;
        int max_width = get_dig_cnt(get_max(digits_count, 10));


        for (size_t k = 0; k < 10; ++k) {
            printf("%ld: %*ld, %5.2lf%c | ", k, max_width, digits_count[k], (double)digits_count[k] / dig_cnt_sum * 100, '%');

            for (size_t j = 0; j < digits_count[k] / mid * 50; ++j) {
                printf("*");
            }

            printf("\n");
        }

        free(digits_count);
    }

    return 0;
}

