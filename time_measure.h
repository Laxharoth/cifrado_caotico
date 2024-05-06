#ifndef TIME_MEASURE_H
#define TIME_MEASURE_H

#include <sys/types.h>
#include <time.h>
#ifndef SAMPLE_SIZE
#define SAMPLE_SIZE 1
#endif

#define time_exec(code)                                                  \
    ({                                                                   \
        struct timespec start_0f915e730828c, end_0f915e730828c;          \
        double elapsed_0f915e730828c;                                    \
        double average_time_0f915e730828c = 0;                           \
        size_t index_0f915e730828c = SAMPLE_SIZE;                        \
        while (index_0f915e730828c--) {                                  \
            clock_gettime(CLOCK_MONOTONIC, &start_0f915e730828c);        \
            code;                                                        \
            clock_gettime(CLOCK_MONOTONIC, &end_0f915e730828c);          \
            elapsed_0f915e730828c = ((end_0f915e730828c.tv_sec * 1e9 +   \
                                      end_0f915e730828c.tv_nsec) -       \
                                     (start_0f915e730828c.tv_sec * 1e9 + \
                                      start_0f915e730828c.tv_nsec)) /    \
                                    1e9;                                 \
            average_time_0f915e730828c += elapsed_0f915e730828c;         \
        }                                                                \
        average_time_0f915e730828c / SAMPLE_SIZE;                        \
    })

#define time_exec_gettimeofday(code)                                     \
    ({                                                                   \
        struct timeval start_0f915e730828c, end_0f915e730828c;           \
        double elapsed_0f915e730828c;                                    \
        double average_time_0f915e730828c = 0;                           \
        size_t index_0f915e730828c = SAMPLE_SIZE;                        \
        while (index_0f915e730828c--) {                                  \
            gettimeofday(&start_0f915e730828c, 0);                       \
            code;                                                        \
            gettimeofday(&end_0f915e730828c, 0);                         \
            elapsed_0f915e730828c = ((end_0f915e730828c.tv_sec * 1e6 +   \
                                      end_0f915e730828c.tv_usec) -       \
                                     (start_0f915e730828c.tv_sec * 1e6 + \
                                      start_0f915e730828c.tv_usec));     \
            average_time_0f915e730828c += elapsed_0f915e730828c / 1e6;   \
        }                                                                \
        average_time_0f915e730828c / SAMPLE_SIZE;                        \
    })

#endif /* TIME_MEASURE_H */