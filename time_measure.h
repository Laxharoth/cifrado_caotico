#ifndef TIME_MEASURE_H
#define TIME_MEASURE_H

#include <sys/types.h>
#include <time.h>

#define print_time(code)                                   \
    {                                                      \
        struct timespec start, end;                        \
        double elapsed;                                    \
        double average_time = 0;                           \
        for (size_t i = 0; i < SAMPLE_SIZE; i++) {         \
            clock_gettime(CLOCK_MONOTONIC, &start);        \
            code;                                          \
            clock_gettime(CLOCK_MONOTONIC, &end);          \
            elapsed = (end.tv_sec - start.tv_sec) +        \
                      (end.tv_nsec - start.tv_nsec) / 1e9; \
            average_time += elapsed;                       \
        }                                                  \
        printf("Tiempo de ejecucion: %.9f segundos\n",     \
               average_time / SAMPLE_SIZE);                \
    }

#endif /* TIME_MEASURE_H */
