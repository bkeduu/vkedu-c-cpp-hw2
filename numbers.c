#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "num_utils.h"

int main(int argc, char* argv[]) {

    clock_t start_time = clock();

    string_t* file_names = NULL;
    size_t files_count = 0;
    size_t proc_count = 0;

    errors_t code = get_params(argv, argc, &file_names, &files_count, &proc_count);

    if(code != 0) {
        print_message(code);
        return 0;
    }

    code = handle_files(file_names, files_count, proc_count);

    if(code != 0) {
        print_message(code);

        for(size_t i = 0; i < files_count; ++i) {
            free(file_names[i]);
        }
        free(file_names);
        return 0;
    }

    for(size_t i = 0; i < files_count; ++i) {
        free(file_names[i]);
    }
    free(file_names);

    clock_t end_time = clock();

    printf("\n\nProgram was executed in %lf second(s).\n", (double)(end_time - start_time) / CLOCKS_PER_SEC);

    return 0;
}
