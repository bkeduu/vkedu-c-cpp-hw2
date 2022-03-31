#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

int main(){

    srand(time(0));
    char* argv[] = { "../../build/vec_1.txt", "../../build/vec_2.txt" };
    pid_t child = 0;
    FILE* vec_1;
    FILE* vec_2;
    clock_t start;
    clock_t end;
    int tmp;

    printf("Building static-library version.\n");
    system("rm -rf ../../build && mkdir -p ../../build && cd ../../build && cmake -DBUILD_STATIC=ON ..  && cmake --build . ");

    printf("Testing static version.\n");
    printf("Parameters: 2 files with 100000 numbers each.\n");
    printf("Generating files.\n");

    vec_1 = fopen("../../build/vec_1.txt", "w+");
    vec_2 = fopen("../../build/vec_2.txt", "w+");

    tmp = 0;

    for(size_t i = 0; i < 1000000; ++i) {
        tmp = rand();
        fprintf(vec_1, "%d\n", tmp);
        tmp = rand();
        fprintf(vec_2, "%d\n", tmp);
    }

    fclose(vec_1);
    fclose(vec_2);

    start = clock();
    end = 0;

    printf("Start clock() value: %ld.\nStarting program.\n", start);

    child = fork();

    if(child == 0) {
        system("./../../build/numbers ../../build/vec_1.txt ../../build/vec_2.txt");
        exit(0);
    }

    waitpid(child, NULL, 0);

    end = clock();

    printf("End clock() value: %ld.\n Time in seconds: %lf\n", end, (end - start) / (double)CLOCKS_PER_SEC);

    printf("Cleaning build directory and building shared-library version.\n");
    system("rm -rf ../../build && mkdir -p ../../build && cd ../../build && cmake -DBUILD_SHARED=ON ..  && cmake --build . ");

    printf("Testing shared version.\n");
    printf("Parameters: 2 files with 100000 numbers each.\n");
    printf("Generating files.\n");

    vec_1 = fopen("../../build/vec_1.txt", "w+");
    vec_2 = fopen("../../build/vec_2.txt", "w+");

    tmp = 0;

    for(size_t i = 0; i < 1000000; ++i) {
        tmp = rand();
        fprintf(vec_1, "%d\n", tmp);
        tmp = rand();
        fprintf(vec_2, "%d\n", tmp);
    }

    fclose(vec_1);
    fclose(vec_2);

    start = clock();
    end = 0;

    printf("Start clock() value: %ld.\nStarting program.\n", start);

    child = fork();

    if(child == 0) {
        system("./../../build/numbers ../../build/vec_1.txt ../../build/vec_2.txt");
        exit(0);
    }

    waitpid(child, NULL, 0);

    end = clock();

    printf("End clock() value: %ld.\n Time in seconds: %lf\n", end, (double)(end - start) / CLOCKS_PER_SEC);


    return 0;
}
