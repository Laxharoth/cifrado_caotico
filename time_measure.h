#ifndef TIME_MEASURE_H
#define TIME_MEASURE_H

#include <sys/types.h>
#include <time.h>
#ifndef SAMPLE_SIZE
#define SAMPLE_SIZE 1
#endif

#define print_time(code)                                                    \
    {                                                                       \
        struct timespec start_0f915e730828c, end_0f915e730828c;             \
        double elapsed_0f915e730828c;                                       \
        double average_time_0f915e730828c = 0;                              \
        for (size_t index_0f915e730828c = 0;                                \
             index_0f915e730828c < SAMPLE_SIZE; index_0f915e730828c++) {    \
            clock_gettime(CLOCK_MONOTONIC, &start_0f915e730828c);           \
            code;                                                           \
            clock_gettime(CLOCK_MONOTONIC, &end_0f915e730828c);             \
            elapsed_0f915e730828c =                                         \
                (end_0f915e730828c.tv_sec - start_0f915e730828c.tv_sec) +   \
                (end_0f915e730828c.tv_nsec - start_0f915e730828c.tv_nsec) / \
                    1e9;                                                    \
            average_time_0f915e730828c += elapsed_0f915e730828c;            \
        }                                                                   \
        printf("Tiempo de ejecucion: %.9f segundos\n",                      \
               average_time_0f915e730828c / SAMPLE_SIZE);                   \
    }

#endif /* TIME_MEASURE_H */
