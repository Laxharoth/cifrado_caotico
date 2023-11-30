#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "ConfigStructure.h"
#include "bitarray.c"
#include "fn.c"
#include "time_measure.h"

#define SAMPLE_SIZE 1
#define MAX_ULL 0xFFFFFFFFFFFFFFFF

/*
    Las funciones generate_random_file
        de 1-3 utilizan arreglos para varias condiciones iniciales

        los de 4 - 7 utilizan  solo un valor como condicion inicial
*/

void generate_random_file_1(const char *file_path,
                            const Configuracion *config) {
    typedef unsigned long long ull;
    FILE *file = fopen(file_path, "wb");
    if (file == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }
    // Obtener condiciones iniciales
    ull X[NUMBER_OF_CAHOTIC_MAPS];
    for (size_t i = 0; i < NUMBER_OF_CAHOTIC_MAPS * 2; ++i) {
        ((uint32_t *)X)[i] = rand();
    }

    const size_t chunk_size =
        sizeof(ull) *
        NUMBER_OF_CAHOTIC_MAPS;  // Tamaño de cada fragmento a escribir
    size_t remaining_bytes = config->file_size;
    while (remaining_bytes > 0) {
        // Generar datos aleatorios para el fragmento actual
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;
        renyi_array_generator(X, config->beta, config->lambda);

#ifndef MEASURE_TIME_ONLY
        fwrite(X, sizeof(unsigned char), chunk_bytes, file);
#endif
        remaining_bytes -= chunk_bytes;
    }

    fclose(file);
}

void generate_random_file_2(const char *file_path,
                            const Configuracion *config) {
    typedef unsigned long long ull;
    FILE *file = fopen(file_path, "wb");
    if (file == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }
    // Obtener condiciones iniciales
    ull X[NUMBER_OF_CAHOTIC_MAPS];
    for (size_t i = 0; i < NUMBER_OF_CAHOTIC_MAPS * 2; ++i) {
        ((uint32_t *)X)[i] = rand();
    }
    ull Yi;

    const size_t chunk_size = sizeof(ull);
    size_t remaining_bytes = config->file_size;
    while (remaining_bytes > 0) {
        // Generar datos aleatorios para el fragmento actual
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;

        Yi = renyi_array_combine_maps_with_mask_and_replace(X, config->beta,
                                                            config->lambda);
#ifndef MEASURE_TIME_ONLY
        fwrite(&Yi, sizeof(unsigned char), chunk_bytes, file);
#endif
        remaining_bytes -= chunk_bytes;
    }

    fclose(file);
}

void generate_random_file_3(const char *file_path,
                            const Configuracion *config) {
    typedef unsigned long long ull;
    FILE *file = fopen(file_path, "wb");
    if (file == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }
    // Obtener condiciones iniciales
    ull X[NUMBER_OF_CAHOTIC_MAPS];
    for (size_t i = 0; i < NUMBER_OF_CAHOTIC_MAPS * 2; ++i) {
        ((uint32_t *)X)[i] = rand();
    }
    ull Yi;

    const size_t chunk_size = sizeof(ull);
    size_t remaining_bytes = config->file_size;
    while (remaining_bytes > 0) {
        // Generar datos aleatorios para el fragmento actual
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;

        Yi = renyi_array_random_byte_select_with_replace(X, config->beta,
                                                         config->lambda);
#ifndef MEASURE_TIME_ONLY
        fwrite(&Yi, sizeof(unsigned char), chunk_bytes, file);
#endif
        remaining_bytes -= chunk_bytes;
    }

    fclose(file);
}

void generate_random_file_4(const char *file_path,
                            const Configuracion *config) {
    typedef unsigned long long ull;
    FILE *file = fopen(file_path, "wb");
    if (file == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }

    srand(config->seed);
    ull X = rand();
    X = (X << 32) | rand();

    const size_t chunk_size =
        sizeof(ull);  // Tamaño de cada fragmento a escribir
    size_t remaining_bytes = config->file_size;
    ull divisor = config->r;
    ull t = (MAX_ULL / divisor);
    t = sqrtull(t);
    while (remaining_bytes > 0) {
        // Generar datos aleatorios para el fragmento actual
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;
        X = logistic_renyi(X, config->beta, config->lambda, config->r, t);

#ifndef MEASURE_TIME_ONLY
        fwrite(&X, sizeof(unsigned char), chunk_bytes, file);
#endif
        remaining_bytes -= chunk_bytes;
    }

    fclose(file);
}

void generate_random_file_5(const char *file_path,
                            const Configuracion *config) {
    typedef unsigned long long ull;
    FILE *file = fopen(file_path, "wb");
    if (file == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }

    srand(config->seed);

    ull X = rand();
    X = (X << 32) | rand();
    ull Yi[255];

    ull size = config->n;

    const size_t chunk_size =
        sizeof(ull) * size;  // Tamaño de cada fragmento a escribir
    size_t remaining_bytes = config->file_size;
    ull t = (MAX_ULL / config->r);
    t = sqrtull(t);
    while (remaining_bytes > 0) {
        // Generar datos aleatorios para el fragmento actual
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;
        X = logistic_renyi_with_cycle(X, Yi, config->beta, config->lambda,
                                      config->r, t, size);

#ifndef MEASURE_TIME_ONLY
        fwrite(&Yi, sizeof(unsigned char), chunk_bytes, file);
#endif
        remaining_bytes -= chunk_bytes;
    }

    fclose(file);
}

void generate_random_file_6(const char *file_path,
                            const Configuracion *config) {
    typedef unsigned long long ull;
    FILE *file = fopen(file_path, "wb");
    if (file == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }

    srand(config->seed);

    ull X = rand();
    X = (X << 32) | rand();
    ull Yi[255];

    ull mask = 0b1111;

    ull divisor = config->r;
    size_t remaining_bytes = config->file_size;
    ull t = (MAX_ULL / divisor);
    t = sqrtull(t);
    while (remaining_bytes > 0) {
        const ull size = (X & mask) | 1;
        const size_t chunk_size =
            sizeof(ull) * size;  // Tamaño de cada fragmento a escribir
        // Generar datos aleatorios para el fragmento actual
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;
        X = logistic_renyi_with_random_cycle(
            X, Yi, config->beta, config->lambda, config->r, t, mask);

#ifndef MEASURE_TIME_ONLY
        fwrite(&Yi, sizeof(unsigned char), chunk_bytes, file);
#endif
        remaining_bytes -= chunk_bytes;
    }

    fclose(file);
}

void generate_random_file_7(const char *file_path,
                            const Configuracion *config) {
    typedef unsigned long long ull;
    FILE *file = fopen(file_path, "wb");
    if (file == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }

    srand(config->seed);

    ull X = rand();
    X = (X << 32) | rand();

    const size_t chunk_size =
        sizeof(ull);  // Tamaño de cada fragmento a escribir
    size_t remaining_bytes = config->file_size;
    ull divisor = config->r;
    ull t = (MAX_ULL / divisor);
    t = sqrtull(t);
    while (remaining_bytes > 0) {
        // Generar datos aleatorios para el fragmento actual
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;
        X = logistic_generalized(X, config->h, config->k, config->n);

#ifndef MEASURE_TIME_ONLY
        fwrite(&X, sizeof(unsigned char), chunk_bytes, file);
#endif
        remaining_bytes -= chunk_bytes;
    }

    fclose(file);
}

void generate_random_file_8(const char *file_path,
                            const Configuracion *config) {
    typedef unsigned long long ull;
    FILE *file = fopen(file_path, "wb");
    if (file == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }
    const size_t numeroMapas = 4;

    ull Xn[numeroMapas], parametros[numeroMapas], epsilon = 65535, j = 5, H = 0;

    srand(config->seed);
    for (size_t i = 0; i < numeroMapas; i++) {
        Xn[i] = rand();
        Xn[i] = (Xn[i] << 32) | rand();
        parametros[i] = (rand() % 3000) + 5;
    }

    const size_t chunk_size =
        sizeof(ull) * numeroMapas;  // Tamaño de cada fragmento a escribir
    size_t remaining_bytes = config->file_size;
    while (remaining_bytes > 0) {
        // Generar datos aleatorios para el fragmento actual
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;
        SecureReal_TimeChaoticPartialEncryptionGenerator(
            Xn, parametros, j, epsilon, &H, numeroMapas);

#ifndef MEASURE_TIME_ONLY
        fwrite(Xn, sizeof(unsigned char), chunk_bytes, file);
#endif
        remaining_bytes -= chunk_bytes;
    }

    fclose(file);
}

void generate_random_file_9(const char *file_path,
                            const Configuracion *config) {
    typedef unsigned long long ull;
    FILE *file = fopen(file_path, "wb");
    if (file == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }

    srand(config->seed);
    ull *rng_generated_buffer = (ull *)malloc(config->file_size);
    ull X = rand();
    X = (X << 32) | rand();

    const size_t chunk_size =
        sizeof(ull);  // Tamaño de cada fragmento a escribir
    size_t remaining_bytes = config->file_size;
    ull index = 0;
    while (remaining_bytes > 0) {
        // Generar datos aleatorios para el fragmento actual
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;
        rng_generated_buffer[index++] = X =
            RenyiMap(X, config->beta, config->lambda);

        remaining_bytes -= chunk_bytes;
    }
#ifndef MEASURE_TIME_ONLY
    fwrite(rng_generated_buffer, sizeof(unsigned char), config->file_size,
           file);
#endif
    free(rng_generated_buffer);
    fclose(file);
}

void generate_random_file_10(const char *file_path,
                             const Configuracion *config) {
    typedef unsigned long long ull;
    FILE *file = fopen(file_path, "wb");
    if (file == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }

    srand(config->seed);
    ull *rng_generated_buffer = (ull *)malloc(config->file_size);
    ull X = rand();
    X = (X << 32) | rand();

    const size_t chunk_size =
        sizeof(ull);  // Tamaño de cada fragmento a escribir
    size_t remaining_bytes = config->file_size;
    ull t = (MAX_ULL / config->r);
    t = sqrtull(t);
    ull index = 0;
    while (remaining_bytes > 0) {
        // Generar datos aleatorios para el fragmento actual
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;
        rng_generated_buffer[index++] = X = LogisticMapInt(X, config->r, t);

        remaining_bytes -= chunk_bytes;
    }
#ifndef MEASURE_TIME_ONLY
    fwrite(rng_generated_buffer, sizeof(unsigned char), config->file_size,
           file);
#endif
    free(rng_generated_buffer);
    fclose(file);
}

void generate_random_file_11(const char *file_path,
                             const Configuracion *config) {
    typedef unsigned long long ull;
    FILE *file = fopen(file_path, "wb");
    if (file == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }
    const size_t numeroMapas = 4;
    ull *rng_generated_buffer = (ull *)malloc(config->file_size);
    ull Xn[numeroMapas], parametros[numeroMapas], j = 5;

    srand(config->seed);
    for (size_t i = 0; i < numeroMapas; i++) {
        Xn[i] = rand();
        Xn[i] = (Xn[i] << 32) | rand();
        parametros[i] = (rand() % 3000) + 5;
    }

    const size_t chunk_size =
        sizeof(ull);  // Tamaño de cada fragmento a escribir
    size_t remaining_bytes = config->file_size;
    ull position = 0;
    ull index = 0;
    while (remaining_bytes > 0) {
        // Generar datos aleatorios para el fragmento actual
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;
        rng_generated_buffer[index++] = random_select_coupled_chaotic_map(
            &position, Xn, parametros, j, 0b111);

        remaining_bytes -= chunk_bytes;
    }
#ifndef MEASURE_TIME_ONLY
    fwrite(rng_generated_buffer, sizeof(unsigned char), config->file_size,
           file);
#endif

    free(rng_generated_buffer);
    fclose(file);
}

void generate_random_file_12(const char *file_path,
                             const Configuracion *config) {
    typedef unsigned long long ull;
    FILE *file = fopen(file_path, "wb");
    if (file == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }
    const size_t numeroMapas = 4;
    ull *rng_generated_buffer = (ull *)malloc(config->file_size);
    ull Xn[numeroMapas], parametros[numeroMapas], epsilon = 65535, j = 5, H = 0;

    srand(config->seed);
    for (size_t i = 0; i < numeroMapas; i++) {
        Xn[i] = rand();
        Xn[i] = (Xn[i] << 32) | rand();
        parametros[i] = (rand() % 3000) + 5;
    }

    const size_t chunk_size =
        sizeof(ull);  // Tamaño de cada fragmento a escribir
    size_t remaining_bytes = config->file_size;
    ull position = 0;
    ull index = 0;
    while (remaining_bytes > 0) {
        // Generar datos aleatorios para el fragmento actual
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;
        rng_generated_buffer[index++] = random_select_coupled_chaotic_map_with_perturbation(
            &position, Xn, parametros, j, epsilon, &H, 0b111);

        remaining_bytes -= chunk_bytes;
    }
#ifndef MEASURE_TIME_ONLY
    fwrite(rng_generated_buffer, sizeof(unsigned char), config->file_size,
           file);
#endif

    free(rng_generated_buffer);
    fclose(file);
}


int main() {
    const Configuracion config = readConfigFile("config.txt");
    char bitarray[10];
    readBitArrayFromFile("rng_selector", bitarray, 80);
    printf("%d\n", bitarray[0]);
    printf("%d\n", bitarray[1]);

    if (ON(bitarray, 1)) {
        print_time({
            const char *file_path = "random_data1.bin";
            generate_random_file_1(file_path, &config);
        });
    }
    if (ON(bitarray, 2)) {
        print_time({
            const char *file_path = "random_data2.bin";
            generate_random_file_2(file_path, &config);
        });
    }
    if (ON(bitarray, 3)) {
        print_time({
            const char *file_path = "random_data3.bin";
            generate_random_file_3(file_path, &config);
        });
    }
    if (ON(bitarray, 4)) {
        print_time({
            const char *file_path = "random_data4.bin";
            generate_random_file_4(file_path, &config);
        });
    }
    if (ON(bitarray, 5)) {
        print_time({
            const char *file_path = "random_data5.bin";
            generate_random_file_5(file_path, &config);
        });
    }
    if (ON(bitarray, 6)) {
        print_time({
            const char *file_path = "random_data6.bin";
            generate_random_file_6(file_path, &config);
        });
    }
    if (ON(bitarray, 7)) {
        print_time({
            const char *file_path = "random_data7.bin";
            generate_random_file_7(file_path, &config);
        });
    }
    if (ON(bitarray, 8)) {
        print_time({
            const char *file_path = "random_data8.bin";
            generate_random_file_8(file_path, &config);
        });
    }
    if (ON(bitarray, 9)) {
        print_time({
            const char *file_path = "random_data9.bin";
            generate_random_file_9(file_path, &config);
        });
    }
    if (ON(bitarray, 10)) {
        print_time({
            const char *file_path = "random_data10.bin";
            generate_random_file_10(file_path, &config);
        });
    }
    if (ON(bitarray, 11)) {
        print_time({
            const char *file_path = "random_data11.bin";
            generate_random_file_11(file_path, &config);
        });
    }
    if (ON(bitarray, 12)) {
        print_time({
            const char *file_path = "random_data11.bin";
            generate_random_file_12(file_path, &config);
        });
    }
    return 0;
}