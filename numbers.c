#include <stdio.h>

#include "num_utils.h"

int main(int argc, char* argv[]) {

    string_t* file_names = NULL;
    size_t files_count = 0;
    size_t proc_count = 0;

    error_t code = get_params(argv, argc, &file_names, &files_count, &proc_count);

    if(code != 0) {
        print_message(code);
        return 0;
    }

    code = start(file_names, files_count, proc_count);

    if(code != 0) {
        print_message(code);
    }

    return 0;
}
