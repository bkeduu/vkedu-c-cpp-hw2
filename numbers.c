#include <stdio.h>

#include "num_utils.h"

int main(int argc, char* argv[]) {

    switch(start(argv, argc)) {

        case ERR_MALLOC:
            printf("Malloc error in program, exiting.\n");
            break;
        case ERR_NULL:
            printf("NULL pointer given to function, exiting.\n");
            break;
        case ERR_FOPEN:
            printf("Unable open file(s), exiting.\n");
            break;
        case ERR_MORE_PROC:
            printf("You\'re requiring more processes, that your system can handle. Exiting...\n");
            break;
        case ERR_UNKNWN_PARAM:
            printf("Unknown parameter given, exiting.\n");
            break;
        case ERR_PROC_PARAM:
            printf("Parameter after -j should be a positive number. Exiting...\n");
            break;
        default:
            break;
    }

    return 0;
}
