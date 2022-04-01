#include <stdio.h>
#include <time.h>

#include "num_utils.h"

int main(int argc, char* argv[]) {
    clock_t start_time = clock();
    size_t files_count = 0;
    size_t proc_count = 0;
    string_t* file_names = NULL;
    errors_t error_code = get_params(argv, argc, &file_names, &files_count, &proc_count);

    if (error_code != 0) {
        print_message(error_code);
        return 0;
    }

    error_code = handle_files(file_names, files_count, proc_count);

    if (error_code != 0) {
        print_message(error_code);
        free_fnames(&file_names, files_count);
        return 0;
    }

    clock_t end_time = clock();
    printf("\n\nProgram was executed in %lf second(s).\n", (double)(end_time - start_time) / CLOCKS_PER_SEC);

    free_fnames(&file_names, files_count);

    return 0;
}
