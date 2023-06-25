#ifdef __x86_64__
#define SYSTEM_BITS 64
#else
#define SYSTEM_BITS 32
#endif

#include <math.h>
#include <stdio.h>
#include <time.h>

#define CASES 255
#define SAMPLE_SIZE 100000

#define print_time(code)                                   \
    {                                                      \
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
typedef unsigned long long ull;

inline long long exp_taylor(long long x) {
    long long result = 1;
    long long tmp = x;
    result += tmp;
    tmp = (x * tmp) >> 1;
    result += tmp;
    tmp *= x / 3;
    result += tmp;
    tmp = (x * tmp) >> 2;
    result += tmp;
    return result;
}

inline double exp_taylor_lf(double x) {
    double result = 1;
    double tmp = x;
    result += tmp;
    tmp = (x * tmp) / 2;
    result += tmp;
    tmp *= x / 3;
    result += tmp;
    tmp = (x * tmp) / 4;
    result += tmp;
    return result;
}

inline ull sin_taylor(ull x) {
    ull resultado_positivo = 0;
    ull resultado_negativo = 0;
    ull x_cuadrado = x * x;
    ull termino_positivo = x;
    ull termino_negativo = x_cuadrado * x / 6;
    int i;

    for (i = 1; i <= 4; i++) {
        resultado_positivo += termino_positivo;
        resultado_negativo += termino_negativo;

        termino_positivo *= x_cuadrado / ((2 * i) * (2 * i + 1));
        termino_negativo *= x_cuadrado / ((2 * (i + 2)) * (2 * (i + 2) + 1));
    }

    if (resultado_positivo > resultado_negativo) {
        return resultado_positivo - resultado_negativo;
    } else {
        return resultado_negativo - resultado_positivo;
    }
}

int main() {
    struct timespec start, end;
    double elapsed;

    exp_taylor(9);

    long long real[CASES], aprox[CASES];
    double aprox_lf[CASES];
    double average_time;

    average_time = 0;
    for (size_t i = 0; i < SAMPLE_SIZE; i++) {
        clock_gettime(CLOCK_MONOTONIC, &start);
        for (long long *ptr_real = real, i = 0; i < CASES; ptr_real++, i++) {
            *ptr_real = exp(i);
        }
        clock_gettime(CLOCK_MONOTONIC, &end);
        elapsed =
            (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        average_time += elapsed;
    }
    printf("Tiempo de ejecucion: %.9f segundos\n", average_time / SAMPLE_SIZE);

    average_time = 0;
    for (size_t i = 0; i < SAMPLE_SIZE; i++) {
        clock_gettime(CLOCK_MONOTONIC, &start);
        for (long long *ptr_aprox = aprox, i = 0; i < CASES; ptr_aprox++, i++) {
            *ptr_aprox = exp_taylor(i);
        }
        clock_gettime(CLOCK_MONOTONIC, &end);
        elapsed =
            (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        average_time += elapsed;
    }

    printf("Tiempo de ejecucion: %.9f segundos\n", average_time / SAMPLE_SIZE);

    long long error = 0;

    for (long long *ptr_aprox = aprox, *ptr_real = real, i = 0; i < CASES;
         ptr_aprox++, ptr_real++, i++) {
        const long long error_i = *ptr_aprox - *ptr_real;
        error += abs(error_i);
    }
    printf("Error de aproximacion: %lld\n", error);

    average_time = 0;
    for (size_t i = 0; i < SAMPLE_SIZE; i++) {
        /* code */

        clock_gettime(CLOCK_MONOTONIC, &start);
        for (double *ptr_aprox = aprox_lf, i = 0; i < CASES; ptr_aprox++, i++) {
            *ptr_aprox = exp_taylor_lf(i);
        }
        clock_gettime(CLOCK_MONOTONIC, &end);
        elapsed =
            (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        average_time += elapsed;
    }
    printf("Tiempo de ejecucion: %.9f segundos\n", average_time / SAMPLE_SIZE);

    double error_lf = 0;

    for (long long i = 0; i < CASES; i++) {
        const double error_i = aprox_lf[i] - real[i];
        error_lf += fabs(error_i);
    }
    printf("Error de aproximacion: %lld\n", error);
    print_time(for (ull i = 0, value; i < CASES; i++) {
        value = sin_taylor(i);
    }) FILE *f = fopen("zin_values.txt", "w");
    for (ull i = 0, value; i < CASES; i++) {
        value = sin_taylor(i);
        fprintf(f, "%llu\n", value);
    }
    fclose(f);
    return 0;
}
