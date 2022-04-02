#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>

#include "num_manip.h"
#include "num_utils.h"

short mp_sort(array_t** arrays, size_t count, size_t proc_count) {  // multiprocess sorting algorithm
    array_t* sh_arrays = mmap(NULL, sizeof(array_t) * count, PROT_READ | PROT_WRITE,
                              MAP_ANONYMOUS | MAP_SHARED, -1 , 0);  // mapping arrays for common use

    if (sh_arrays == (void*)-1) {  // if mapping was unsuccessful
        return ERR_PROC;
    }

    memcpy(sh_arrays, *arrays, sizeof(array_t) * count);  // NOLINT because secure memcpy_s windows-only
    errors_t error_code;

    pid_t* pids = malloc(sizeof(pid_t) * proc_count);  // ptr to array with child processes pids

    if (pids == NULL) {
        munmap(sh_arrays, sizeof(array_t) * count);
        return ERR_MALLOC;
    }

    for (size_t i = 0; i < proc_count; ++i) {
        pids[i] = 0;
    }

    for (size_t i = 0; i < proc_count; ++i) {
        pids[i] = fork();  // create new process

        if (pids[i] < 0) {  // error while creating
            munmap(sh_arrays, sizeof(array_t) * count);
            free(pids);
            return ERR_PROC;
        } else if (pids[i] == 0) {  // part with child process code
            for (size_t k = i; k < count; k += proc_count) {
                qsort((*arrays)[k].arr, (*arrays)[k].size,  // sorting only arrays,
                                                                // which index % proc_index == 0
                      sizeof(int), (int(*)(const void*, const void*)) comp);
            }

            free(pids);
            return NERROR_PROC_EXIT;  // no error, just code for returning in another functions
                                                                    // to clean all heap memory
        }
    }

    for (size_t i = 0; i < proc_count; ++i) {  // waiting all child processes in parent process
        waitpid(pids[i], NULL, 0);
    }

    free(pids);
    memcpy(*arrays, sh_arrays, sizeof(array_t) * count);  // NOLINT because secure memcpy windows-only
    error_code = munmap(sh_arrays, sizeof(array_t) * count);  // unmapping common memory

    if (error_code) {
        return ERR_PROC;
    }

    return 0;
}
