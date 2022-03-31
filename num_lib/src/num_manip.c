#include <string.h>
#include <math.h>

#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>

#include "num_manip.h"
#include "num_utils.h"

int get_max(const size_t* arr, size_t size) {
    if(arr == NULL || size == 0) {
        return ERR_NULL;
    }

    int max = (int)arr[0];

    for(size_t i = 0; i < size; ++i) {
        if(max < arr[i]) {
            max = arr[i];
        }
    }
    return max;
}

long get_sum(const size_t* arr, size_t size) {
    if(arr == NULL) {
        return ERR_NULL;
    }

    int sum = 0;

    for(size_t i = 0; i < size; ++i) {
        sum += arr[i];
    }
    return sum;
}

size_t get_dig_cnt(int num) {
    return ceil(log10((num > 0) ? num : -num));
}

int comp(const int* a, const int* b) {
    return *a - *b;
}

short m_sort(array_t** arrays, size_t count, size_t proc_count) {

    array_t* sh_arrays = mmap(NULL, sizeof(array_t) * count, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1 ,0);
    memcpy(sh_arrays, *arrays, sizeof(array_t) * count);

    pid_t* pids = malloc(sizeof(pid_t) * proc_count);

    if(pids == NULL) {

        munmap(sh_arrays, sizeof(array_t) * count);
        return ERR_MALLOC;
    }

    for(size_t i = 0; i < proc_count; ++i) {
        pids[i] = 0;
    }

    for(size_t i = 0; i < proc_count; ++i) {
        pids[i] = fork();

        if (pids[i] < 0) {
            free(pids);
            return ERR_PROC;
        }
        else if (pids[i] == 0) {
            for (size_t k = i; k < count; k += proc_count) {
                qsort((*arrays)[k].arr, (*arrays)[k].size, sizeof(int), (int(*)(const void*, const void*)) comp);
            }

            free(pids);
            return NERROR_PROC_EXIT;
        }
    }

    for(size_t i = 0; i < proc_count; ++i) {
        waitpid(pids[i], NULL, 0);
    }

    free(pids);
    memcpy(*arrays, sh_arrays, sizeof(array_t) * count);
    munmap(sh_arrays, sizeof(array_t) * count);

    return 0;
}
