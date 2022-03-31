#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "num_utils.h"

int main(int argc, char* argv[]) {

    clock_t start_time = clock();
    errors_t code = handle_files(argv, argc);

    if(code != 0) {
        print_message(code);
        return 0;
    }

    clock_t end_time = clock();
    printf("\n\nProgram was executed in %lf second(s).\n", (double)(end_time - start_time) / CLOCKS_PER_SEC);

    return 0;
}
