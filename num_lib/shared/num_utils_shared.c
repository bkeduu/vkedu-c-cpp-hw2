#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>

#include <sys/mman.h>
#include <sys/wait.h>

#include "num_utils.h"
#include "num_manip.h"

errors_t handle_files(string_t* file_names, size_t files_count, size_t proc_count) {
    errors_t code;

    if (file_names == NULL || files_count == 0) {
        return ERR_NULL;
    }

    if (proc_count > sysconf(_SC_NPROCESSORS_ONLN)) {
        return ERR_MORE_PROC;
    }

    if (proc_count == 0) {
        proc_count = sysconf(_SC_NPROCESSORS_ONLN);
    }

    array_t* arrays = malloc(sizeof(array_t) * files_count);
    for (size_t i = 0; i < files_count; ++i) {
        arrays[i].arr = NULL;
        arrays[i].size = 0;
        arrays[i].vec_name = NULL;
    }

    for (size_t i = 0; i < files_count; ++i) {
        code = get_vector(&arrays[i], file_names[i]);

        if (code != 0) {
            free_arrays(&arrays, files_count);
            return code;
        }
    }

    code = mp_sort(&arrays, files_count, proc_count);

    if (code != 0) {
        free_arrays(&arrays, files_count);
        return code;
    }

    for (size_t i = 0; i < files_count; ++i) {
        code = print_info(arrays[i], proc_count);

        if (code != 0) {
            free_arrays(&arrays, files_count);
            return code;
        }
    }

    free_arrays(&arrays, files_count);

    return 0;
}

errors_t get_digits_count(array_t* array, size_t proc_count, size_t** digits_count) {
    if (array == NULL) {
        return ERR_NULL;
    }

    short error_code;
    *digits_count = malloc(sizeof(size_t) * DIGITS_COUNT);

    if (*digits_count == NULL) {
        return ERR_MALLOC;
    }

    for (size_t k = 0; k < DIGITS_COUNT; ++k) {
        (*digits_count)[k] = 0;
    }

    size_t* sh_digits_count = mmap(NULL, sizeof(size_t) * DIGITS_COUNT, PROT_WRITE | PROT_READ,
                                   MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (sh_digits_count == (void*)-1) {
        free(*digits_count);
        return ERR_PROC;
    }

    memcpy(sh_digits_count, *digits_count, sizeof(size_t) * DIGITS_COUNT);  // NOLINT
                                            // because secure memcpy windows-only

    sem_t* semaphore = malloc(sizeof(sem_t));

    if (semaphore == NULL) {
        return ERR_MALLOC;
    }

    error_code = (errors_t)sem_init(semaphore, 1, 1);

    if (error_code == -1) {
        munmap(sh_digits_count, sizeof(size_t) * DIGITS_COUNT);
        free(*digits_count);
        return ERR_PROC;
    }

    sem_t* sh_semaphore = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE,
                               MAP_ANONYMOUS | MAP_SHARED, -1, 0);

    if (sh_semaphore == (void*)-1) {
        munmap(sh_digits_count, sizeof(size_t) * DIGITS_COUNT);
        free(semaphore);
        free(*digits_count);
        return ERR_PROC;
    }

    memcpy(sh_semaphore, semaphore, sizeof(sem_t));  // NOLINT because secure memcpy windows-only

    pid_t *pids = malloc(sizeof(pid_t) * proc_count);

    if (pids == NULL) {
        return ERR_MALLOC;
    }

    for (size_t i = 0; i < proc_count; ++i) {
        pids[i] = fork();

        if (pids[i] < 0) {
            free(pids);
            return ERR_PROC;
        } else if (pids[i] == 0) {
            for (size_t k = i; k < array->size; k += proc_count) {
                sem_wait(sh_semaphore);

                int curr_value = array->arr[k];

                while (curr_value > 0) {
                    sh_digits_count[curr_value % DIGITS_COUNT]++;
                    curr_value /= 10;
                }

                sem_post(sh_semaphore);
            }

            sem_destroy(semaphore);
            free(*digits_count);
            free(semaphore);
            free(pids);
            return NERROR_PROC_EXIT;
        }
    }

    for (size_t j = 0; j < proc_count; ++j) {
        waitpid(pids[j], NULL, 0);
    }

    error_code = (errors_t)sem_destroy(semaphore);

    if (error_code == -1) {
        munmap(sh_digits_count, sizeof(size_t) * DIGITS_COUNT);
        munmap(sh_semaphore, sizeof(sem_t));
        free(semaphore);
        free(pids);
        free(*digits_count);
        return ERR_PROC;
    }

    free(semaphore);
    free(pids);

    memcpy(*digits_count, sh_digits_count, sizeof(size_t) * DIGITS_COUNT);  // NOLINT
                                                            // because secure memcpy windows-only
    error_code = (errors_t)munmap(sh_digits_count, sizeof(size_t) * DIGITS_COUNT);

    if (error_code) {
        munmap(sh_semaphore, sizeof(sem_t));
        free(*digits_count);
        return ERR_PROC;
    }

    error_code = (errors_t)munmap(sh_semaphore, sizeof(sem_t));

    if (error_code) {
        free(*digits_count);
        return ERR_PROC;
    }

    return 0;
}
