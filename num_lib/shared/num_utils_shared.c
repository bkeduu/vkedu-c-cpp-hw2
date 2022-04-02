#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>

#include <sys/mman.h>
#include <sys/wait.h>

#include "num_utils.h"
#include "num_manip.h"

errors_t handle_files(string_t* file_names, size_t files_count, size_t proc_count) {  // multiprocess version
            // of function handle_files, that handle file_names array with proc_count processes
    errors_t code;

    if (file_names == NULL || files_count == 0) {
        return ERR_NULL;
    }

    if (proc_count > sysconf(_SC_NPROCESSORS_ONLN)) {  // if given proc_count more that CPUs in system
        return ERR_MORE_PROC;
    }

    if (proc_count == 0) {  // if proc_count is not given, use maximum possible processes
        proc_count = sysconf(_SC_NPROCESSORS_ONLN);
    }

    array_t* arrays = malloc(sizeof(array_t) * files_count);  // ptr to array with arrays-vectors
    for (size_t i = 0; i < files_count; ++i) {
        arrays[i].arr = NULL;
        arrays[i].size = 0;
        arrays[i].vec_name = NULL;
    }

    for (size_t i = 0; i < files_count; ++i) {
        code = get_vector(&arrays[i], file_names[i]);  // scan each vector from file

        if (code != 0) {
            free_arrays(&arrays, files_count);
            return code;
        }
    }

    code = mp_sort(&arrays, files_count, proc_count);  // sort them

    if (code != 0) {
        free_arrays(&arrays, files_count);
        return code;
    }

    for (size_t i = 0; i < files_count; ++i) {
        code = print_info(arrays[i], proc_count);  // and print information about it

        if (code != 0) {
            free_arrays(&arrays, files_count);
            return code;
        }
    }

    free_arrays(&arrays, files_count);

    return 0;
}

errors_t get_digits_count(array_t* array, size_t proc_count, size_t** digits_count) {
                        // multiprocess version of function that returns digits distribution of vector
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
                                   MAP_SHARED | MAP_ANONYMOUS, -1, 0);  // map memory for common use

    if (sh_digits_count == (void*)-1) {  // if mapping was unsuccessful
        free(*digits_count);
        return ERR_PROC;
    }

    memcpy(sh_digits_count, *digits_count, sizeof(size_t) * DIGITS_COUNT);  // NOLINT
                                            // because secure memcpy_s windows-only

    sem_t* semaphore = malloc(sizeof(sem_t));  // semaphore for locking array for single-process only writing

    if (semaphore == NULL) {
        return ERR_MALLOC;
    }

    error_code = (errors_t)sem_init(semaphore, 1, 1);  // init semaphore

    if (error_code == -1) {
        munmap(sh_digits_count, sizeof(size_t) * DIGITS_COUNT);
        free(*digits_count);
        return ERR_PROC;
    }

    sem_t* sh_semaphore = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE,
                               MAP_ANONYMOUS | MAP_SHARED, -1, 0);  // and map him

    if (sh_semaphore == (void*)-1) {
        munmap(sh_digits_count, sizeof(size_t) * DIGITS_COUNT);
        free(semaphore);
        free(*digits_count);
        return ERR_PROC;
    }

    memcpy(sh_semaphore, semaphore, sizeof(sem_t));  // NOLINT because secure memcpy_s windows-only

    pid_t *pids = malloc(sizeof(pid_t) * proc_count);  // ptr to array with child processes pids

    if (pids == NULL) {
        return ERR_MALLOC;
    }

    for (size_t i = 0; i < proc_count; ++i) {
        pids[i] = fork();

        if (pids[i] < 0) {  // error while creating process
            free(pids);
            return ERR_PROC;
        } else if (pids[i] == 0) {  // part with child process code
            for (size_t k = i; k < array->size; k += proc_count) {
                sem_wait(sh_semaphore);  // lock semaphore or wait for his unlock

                int curr_value = array->arr[k];

                while (curr_value > 0) {
                    sh_digits_count[curr_value % DIGITS_COUNT]++;
                    curr_value /= 10;
                }

                sem_post(sh_semaphore);  // unlock semaphore
            }

            sem_destroy(semaphore);  // cleaning all memory used in this function
            free(*digits_count);
            free(semaphore);
            free(pids);
            return NERROR_PROC_EXIT;  // no error, just code for returning in another functions
                                     // to clean all heap memory
        }
    }

    for (size_t j = 0; j < proc_count; ++j) {  // wait children in parent
        waitpid(pids[j], NULL, 0);
    }

    error_code = (errors_t)sem_destroy(semaphore);  // destroy semaphore

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
                                                            // because secure memcpy_s windows-only
    error_code = (errors_t)munmap(sh_digits_count, sizeof(size_t) * DIGITS_COUNT);  // unmap result array

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
