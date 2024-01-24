#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
#ifndef MEASURE_TIME_ONLY
    FILE *file = fopen(file_path, "wb");
    if (file == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }
#endif

    // Obtener condiciones iniciales
    ull X[NUMBER_OF_CAHOTIC_MAPS];
    for (size_t i = 0; i < NUMBER_OF_CAHOTIC_MAPS * 2; ++i) {
        ((uint32_t *)X)[i] = rand();
    }

    unsigned char *const buffer = (unsigned char *)malloc(config->file_size);
    unsigned char *ptr_buffer = buffer;

    const size_t chunk_size =
        sizeof(ull) *
        NUMBER_OF_CAHOTIC_MAPS;  // Tamaño de cada fragmento a escribir
    size_t remaining_bytes = config->file_size;
    while (remaining_bytes > 0) {
        // Generar datos aleatorios para el fragmento actual
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;
        renyi_array_generator(X, config->beta, config->lambda);
        memcpy(ptr_buffer, X, chunk_bytes);
        ptr_buffer += chunk_bytes;

        remaining_bytes -= chunk_bytes;
    }
#ifndef MEASURE_TIME_ONLY
    fwrite(buffer, config->file_size, 1, file);
    fclose(file);
#endif
    free(buffer);
}

void generate_random_file_2(const char *file_path,
                            const Configuracion *config) {
    typedef unsigned long long ull;
#ifndef MEASURE_TIME_ONLY
    FILE *file = fopen(file_path, "wb");
    if (file == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }
#endif

    // Obtener condiciones iniciales
    ull X[NUMBER_OF_CAHOTIC_MAPS];
    for (size_t i = 0; i < NUMBER_OF_CAHOTIC_MAPS * 2; ++i) {
        ((uint32_t *)X)[i] = rand();
    }
    ull Yi;

    unsigned char *const buffer = (unsigned char *)malloc(config->file_size);
    unsigned char *ptr_buffer = buffer;

    const size_t chunk_size = sizeof(ull);
    size_t remaining_bytes = config->file_size;
    while (remaining_bytes > 0) {
        // Generar datos aleatorios para el fragmento actual
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;

        Yi = renyi_array_combine_maps_with_mask_and_replace(X, config->beta,
                                                            config->lambda);
        memcpy(ptr_buffer, &Yi, chunk_bytes);
        ptr_buffer += chunk_bytes;
        remaining_bytes -= chunk_bytes;
    }
#ifndef MEASURE_TIME_ONLY
    fwrite(buffer, config->file_size, 1, file);
    fclose(file);
#endif
    free(buffer);
}

void generate_random_file_3(const char *file_path,
                            const Configuracion *config) {
    typedef unsigned long long ull;
#ifndef MEASURE_TIME_ONLY
    FILE *file = fopen(file_path, "wb");
    if (file == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }
#endif

    // Obtener condiciones iniciales
    ull X[NUMBER_OF_CAHOTIC_MAPS];
    for (size_t i = 0; i < NUMBER_OF_CAHOTIC_MAPS * 2; ++i) {
        ((uint32_t *)X)[i] = rand();
    }
    ull Yi;

    unsigned char *const buffer = (unsigned char *)malloc(config->file_size);
    unsigned char *ptr_buffer = buffer;

    const size_t chunk_size = sizeof(ull);
    size_t remaining_bytes = config->file_size;
    while (remaining_bytes > 0) {
        // Generar datos aleatorios para el fragmento actual
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;

        Yi = renyi_array_random_byte_select_with_replace(X, config->beta,
                                                         config->lambda);
        memcpy(ptr_buffer, &Yi, chunk_bytes);
        ptr_buffer += chunk_bytes;

        remaining_bytes -= chunk_bytes;
    }
#ifndef MEASURE_TIME_ONLY
    fwrite(buffer, config->file_size, 1, file);
    fclose(file);
#endif
    free(buffer);
}

void generate_random_file_4(const char *file_path,
                            const Configuracion *config) {
    typedef unsigned long long ull;
#ifndef MEASURE_TIME_ONLY
    FILE *file = fopen(file_path, "wb");
    if (file == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }
#endif

    srand(config->seed);
    ull X = rand();
    X = (X << 32) | rand();

    unsigned char *const buffer = (unsigned char *)malloc(config->file_size);
    unsigned char *ptr_buffer = buffer;

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
        memcpy(ptr_buffer, &X, chunk_bytes);
        ptr_buffer += chunk_bytes;

        remaining_bytes -= chunk_bytes;
    }
#ifndef MEASURE_TIME_ONLY
    fwrite(buffer, config->file_size, 1, file);
    fclose(file);
#endif
    free(buffer);
}

void generate_random_file_5(const char *file_path,
                            const Configuracion *config) {
    typedef unsigned long long ull;
#ifndef MEASURE_TIME_ONLY
    FILE *file = fopen(file_path, "wb");
    if (file == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }
#endif

    srand(config->seed);

    ull X = rand();
    X = (X << 32) | rand();
    ull Yi[255];
    unsigned char *const buffer = (unsigned char *)malloc(config->file_size);
    unsigned char *ptr_buffer = buffer;

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

        memcpy(ptr_buffer, Yi, chunk_bytes);
        ptr_buffer += chunk_bytes;

        remaining_bytes -= chunk_bytes;
    }

#ifndef MEASURE_TIME_ONLY
    fwrite(buffer, config->file_size, 1, file);
    fclose(file);
#endif
    free(buffer);
}

void generate_random_file_6(const char *file_path,
                            const Configuracion *config) {
    typedef unsigned long long ull;
#ifndef MEASURE_TIME_ONLY
    FILE *file = fopen(file_path, "wb");
    if (file == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }
#endif

    srand(config->seed);

    ull X = rand();
    X = (X << 32) | rand();
    ull Yi[255];
    unsigned char *const buffer = (unsigned char *)malloc(config->file_size);
    unsigned char *ptr_buffer = buffer;
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

        memcpy(ptr_buffer, Yi, chunk_bytes);
        ptr_buffer += chunk_bytes;

        remaining_bytes -= chunk_bytes;
    }

#ifndef MEASURE_TIME_ONLY
    fwrite(buffer, config->file_size, 1, file);
    fclose(file);
#endif
    free(buffer);
}

void generate_random_file_7(const char *file_path,
                            const Configuracion *config) {
    typedef unsigned long long ull;
#ifndef MEASURE_TIME_ONLY
    FILE *file = fopen(file_path, "wb");
    if (file == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }
#endif

    srand(config->seed);

    ull X = rand();
    X = (X << 32) | rand();

    const size_t chunk_size =
        sizeof(ull);  // Tamaño de cada fragmento a escribir
    size_t remaining_bytes = config->file_size;
    ull divisor = config->r;
    ull t = (MAX_ULL / divisor);
    t = sqrtull(t);

    unsigned char *const buffer = (unsigned char *)malloc(config->file_size);
    unsigned char *ptr_buffer = buffer;

    while (remaining_bytes > 0) {
        // Generar datos aleatorios para el fragmento actual
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;
        X = logistic_generalized(X, config->h, config->k, config->n);

        memcpy(ptr_buffer, &X, chunk_bytes);
        ptr_buffer += chunk_bytes;

        remaining_bytes -= chunk_bytes;
    }
#ifndef MEASURE_TIME_ONLY
    fwrite(buffer, config->file_size, 1, file);
    fclose(file);
#endif
    free(buffer);
}

void generate_random_file_8(const char *file_path,
                            const Configuracion *config) {
    typedef unsigned long long ull;
#ifndef MEASURE_TIME_ONLY
    FILE *file = fopen(file_path, "wb");
    if (file == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }
#endif

    const size_t numeroMapas = 4;

    ull Xn[numeroMapas], parametros[numeroMapas], epsilon = 65535, j = 5, H = 0;

    srand(config->seed);
    for (size_t i = 0; i < numeroMapas; i++) {
        Xn[i] = rand();
        Xn[i] = (Xn[i] << 32) | rand();
        parametros[i] = (rand() % 3000) + 5;
    }

    unsigned char *const buffer = (unsigned char *)malloc(config->file_size);
    unsigned char *ptr_buffer = buffer;

    const size_t chunk_size =
        sizeof(ull) * numeroMapas;  // Tamaño de cada fragmento a escribir
    size_t remaining_bytes = config->file_size;
    while (remaining_bytes > 0) {
        // Generar datos aleatorios para el fragmento actual
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;
        SecureReal_TimeChaoticPartialEncryptionGenerator(
            Xn, parametros, j, epsilon, &H, numeroMapas);

        memcpy(ptr_buffer, Xn, chunk_bytes);
        ptr_buffer += chunk_bytes;

        remaining_bytes -= chunk_bytes;
    }
#ifndef MEASURE_TIME_ONLY
    fwrite(Xn, config->file_size, 1, file);
    fclose(file);
#endif
    free(buffer);
}

void generate_random_file_9(const char *file_path,
                            const Configuracion *config) {
    typedef unsigned long long ull;
#ifndef MEASURE_TIME_ONLY
    FILE *file = fopen(file_path, "wb");
    if (file == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }
#endif

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
    fclose(file);
#endif
    free(rng_generated_buffer);
}

void generate_random_file_10(const char *file_path,
                             const Configuracion *config) {
    typedef unsigned long long ull;
#ifndef MEASURE_TIME_ONLY
    FILE *file = fopen(file_path, "wb");
    if (file == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }
#endif

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
    fclose(file);
#endif
    free(rng_generated_buffer);
}

void generate_random_file_11(const char *file_path,
                             const Configuracion *config) {
    typedef unsigned long long ull;
#ifndef MEASURE_TIME_ONLY
    FILE *file = fopen(file_path, "wb");
    if (file == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }
#endif

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
            &position, Xn, parametros, j, 0b11);

        remaining_bytes -= chunk_bytes;
    }
#ifndef MEASURE_TIME_ONLY
    fwrite(rng_generated_buffer, sizeof(unsigned char), config->file_size,
           file);
    fclose(file);
#endif

    free(rng_generated_buffer);
}

void generate_random_file_12(const char *file_path,
                             const Configuracion *config) {
    typedef unsigned long long ull;
#ifndef MEASURE_TIME_ONLY
    FILE *file = fopen(file_path, "wb");
    if (file == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }
#endif

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
        rng_generated_buffer[index++] =
            random_select_coupled_chaotic_map_with_perturbation(
                &position, Xn, parametros, j, epsilon, &H, 0b11);

        remaining_bytes -= chunk_bytes;
    }
#ifndef MEASURE_TIME_ONLY
    fwrite(rng_generated_buffer, sizeof(unsigned char), config->file_size,
           file);
    fclose(file);
#endif

    free(rng_generated_buffer);
}
void generate_random_file_13(const char *file_path,
                             const Configuracion *config) {
    typedef unsigned long long ull;
#ifndef MEASURE_TIME_ONLY
    FILE *file = fopen(file_path, "wb");
    if (file == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }
#endif

    const size_t numeroMapas = 4;
    const ull num_mapas_mask = 0b11;
    unsigned char *const rng_generated_buffer =
        (unsigned char *)malloc(config->file_size);
    unsigned char *rng_generated_buffer_write = rng_generated_buffer;
    ull Xn[numeroMapas * config->n], parametros[numeroMapas], epsilon = 65535,
                                                              lambda = 5, H = 0;
    chaotic_lookup_table roulete[numeroMapas];
    srand(config->seed);
    const ull table_mask = config->n - 1;
    for (size_t i = 0; i < numeroMapas; i++) {
        Xn[i * config->n] = rand();
        Xn[i * config->n] = (Xn[i] << 32) | rand();
        parametros[i] = (rand() % 3000) + 5;
        for (size_t j = 1; j < config->n; ++j) {
            Xn[i * config->n + j] =
                RenyiMap(Xn[i * config->n + j - 1], parametros[i], lambda);
        }
        roulete[i].lookup_table = Xn;
        roulete[i].lu_table_size = config->n;
        roulete[i].lu_table_mask = table_mask;
        roulete[i].last_generated = Xn[config->n - 1];
    }

    const size_t chunk_size =
        sizeof(ull);  // Tamaño de cada fragmento a escribir
    size_t remaining_bytes = config->file_size;
    ull position = 0;
    ull index = 0;

    ull lu_table_position_list[config->n];
    const ull increase_shift_lut_pos_list = countBitsSet(table_mask);
    ull current_shift_lut_pos_list;
    ull i_lut_pos_list_index;
    ull select_map_for_lut_pos_list = 0;
    ull select_indx_for_lut_pos_list = 0;
    while (remaining_bytes > 0) {
        // Crear lista de numeros aleatorios para seleccionar el indice del mapa
        const size_t table_1 = roulete[select_map_for_lut_pos_list]
                                   .lookup_table[select_indx_for_lut_pos_list] &
                               num_mapas_mask;
        select_map_for_lut_pos_list =
            (select_map_for_lut_pos_list + 1) & num_mapas_mask;
        select_indx_for_lut_pos_list =
            (select_indx_for_lut_pos_list + 1) & table_mask;
        const size_t table_2 = roulete[select_map_for_lut_pos_list]
                                   .lookup_table[select_indx_for_lut_pos_list] &
                               num_mapas_mask;
        select_map_for_lut_pos_list =
            (select_map_for_lut_pos_list + 1) & num_mapas_mask;
        select_indx_for_lut_pos_list =
            (select_indx_for_lut_pos_list + 1) & table_mask;
        for (ull i = 0; i < config->n; ++i) {
            lu_table_position_list[i] = roulete[table_1].lookup_table[i] ^
                                        roulete[table_2].lookup_table[i];
        }

        for (i_lut_pos_list_index = 0;
             i_lut_pos_list_index < config->n && remaining_bytes;
             ++i_lut_pos_list_index) {
            current_shift_lut_pos_list = 0;
            for (ull current_shift_lut_pos_list = 0;
                 current_shift_lut_pos_list < (sizeof(ull) << 3) &&
                 remaining_bytes;
                 current_shift_lut_pos_list += increase_shift_lut_pos_list) {
                size_t chunk_bytes = (chunk_size < remaining_bytes)
                                         ? chunk_size
                                         : remaining_bytes;
                const ull generated =
                    random_select_coupled_chaotic_map_lookuptable(
                        &position,
                        (lu_table_position_list[i_lut_pos_list_index] >>
                         current_shift_lut_pos_list) &
                            table_mask,
                        roulete, parametros, lambda, num_mapas_mask, epsilon,
                        &H);
                memcpy(rng_generated_buffer_write, &generated, chunk_bytes);
                rng_generated_buffer_write += chunk_bytes;
                remaining_bytes -= chunk_bytes;
            }
        }
    }
#ifndef MEASURE_TIME_ONLY
    fwrite(rng_generated_buffer, sizeof(unsigned char), config->file_size,
           file);
    fclose(file);
#endif

    free(rng_generated_buffer);
}
void generate_random_file_14(const char *file_path,
                             const Configuracion *config) {
    typedef unsigned long long ull;
#ifndef MEASURE_TIME_ONLY
    FILE *file = fopen(file_path, "wb");
    if (file == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }
#endif

    const size_t numeroMapas = 4;
    const ull num_mapas_mask = 0b11;
    const ull table_mask = (config->n << 3) - 1;
    unsigned char *const rng_generated_buffer =
        (unsigned char *)malloc(config->file_size);
    unsigned char *rng_generated_buffer_write = rng_generated_buffer;
    ull Xn[numeroMapas * (config->n + 1)], parametros[numeroMapas],
        epsilon = 65535, j = 5, H = 0;
    chaotic_lookup_table roulete[numeroMapas];
    srand(config->seed);
    for (size_t i = 0; i < numeroMapas; i++) {
        Xn[i * (config->n + 1)] = rand();
        Xn[i * (config->n + 1)] = (Xn[i] << 32) | rand();
        parametros[i] = (rand() % 3000) + 5;
        for (size_t j = 1; j < (config->n + 1); ++j) {
            Xn[i * (config->n + 1) + j] =
                RenyiMap(Xn[i * (config->n + 1) + j - 1], parametros[i], j);
        }
        roulete[i].lookup_table = Xn + i * (config->n + 1);
        roulete[i].lu_table_size = config->n;
        roulete[i].lu_table_mask = table_mask;
        roulete[i].last_generated = Xn[i * (config->n + 1) + config->n - 1];
    }

    const size_t chunk_size =
        sizeof(ull);  // Tamaño de cada fragmento a escribir
    size_t remaining_bytes = config->file_size;
    ull position = 0;
    ull index = 0;
    ull lu_table_position_list[config->n];
    const ull increase_shift_lut_pos_list = countBitsSet(table_mask);
    ull current_shift_lut_pos_list;
    ull i_lut_pos_list_index;
    ull select_map_for_lut_pos_list = 0;
    ull select_indx_for_lut_pos_list = 0;
    while (remaining_bytes > 0) {
        // Crear lista de numeros aleatorios para seleccionar el indice del mapa
        const size_t table_1 = roulete[select_map_for_lut_pos_list]
                                   .lookup_table[select_indx_for_lut_pos_list] &
                               num_mapas_mask;
        select_map_for_lut_pos_list =
            (select_map_for_lut_pos_list + 1) & num_mapas_mask;
        select_indx_for_lut_pos_list =
            (select_indx_for_lut_pos_list + 1) & table_mask;
        const size_t table_2 = roulete[select_map_for_lut_pos_list]
                                   .lookup_table[select_indx_for_lut_pos_list] &
                               num_mapas_mask;
        select_map_for_lut_pos_list =
            (select_map_for_lut_pos_list + 1) & num_mapas_mask;
        select_indx_for_lut_pos_list =
            (select_indx_for_lut_pos_list + 1) & table_mask;
        // Generar datos aleatorios para el fragmento actual
        for (ull i = 0; i < config->n; ++i) {
            lu_table_position_list[i] = roulete[table_1].lookup_table[i] ^
                                        roulete[table_2].lookup_table[i];
        }
        // --
        for (i_lut_pos_list_index = 0;
             i_lut_pos_list_index < config->n && remaining_bytes;
             ++i_lut_pos_list_index) {
            current_shift_lut_pos_list = 0;
            for (ull current_shift_lut_pos_list = 0;
                 current_shift_lut_pos_list < (sizeof(ull) << 3) &&
                 remaining_bytes;
                 current_shift_lut_pos_list += increase_shift_lut_pos_list) {
                size_t chunk_bytes = (chunk_size < remaining_bytes)
                                         ? chunk_size
                                         : remaining_bytes;
                const ull generated =
                    random_select_coupled_chaotic_map_lookuptable_byte(
                        &position,
                        (lu_table_position_list[i_lut_pos_list_index] >>
                         current_shift_lut_pos_list) &
                            table_mask,
                        roulete, parametros, j, num_mapas_mask, epsilon, &H);
                memcpy(rng_generated_buffer_write, &generated, chunk_bytes);
                rng_generated_buffer_write += chunk_bytes;
                remaining_bytes -= chunk_bytes;
            }
        }
    }
#ifndef MEASURE_TIME_ONLY
    fwrite(rng_generated_buffer, sizeof(unsigned char), config->file_size,
           file);
    fclose(file);
#endif

    free(rng_generated_buffer);
}

void generate_random_file_15(const char *file_path,
                             const Configuracion *config) {
    typedef unsigned long long ull;
#ifndef MEASURE_TIME_ONLY
    FILE *file = fopen(file_path, "wb");
    if (file == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }
#endif

    const size_t numeroMapas = 4;
    const ull num_mapas_mask = 0b11;
    unsigned char *const rng_generated_buffer =
        (unsigned char *)malloc(config->file_size);
    unsigned char *rng_generated_buffer_write = rng_generated_buffer;
    ull Xn[numeroMapas * config->n], parametros[numeroMapas], epsilon = 65535,
                                                              lambda = 5, H = 0;
    chaotic_lookup_table roulete[numeroMapas];
    srand(config->seed);
    const ull table_mask = config->n - 1;
    for (size_t i = 0; i < numeroMapas; i++) {
        Xn[i * config->n] = rand();
        Xn[i * config->n] = (Xn[i] << 32) | rand();
        parametros[i] = (rand() % 3000) + 5;
        for (size_t j = 1; j < config->n; ++j) {
            Xn[i * config->n + j] =
                RenyiMap(Xn[i * config->n + j - 1], parametros[i], lambda);
        }
        roulete[i].lookup_table = Xn;
        roulete[i].lu_table_size = config->n;
        roulete[i].lu_table_mask = table_mask;
        roulete[i].last_generated = Xn[config->n - 1];
    }

    size_t remaining_bytes = config->file_size;
    ull position = 0;
    ull index = 0;

    const ull increase_shift_lut_pos_list = countBitsSet(table_mask);
    ull current_shift_lut_pos_list;
    ull i_lut_pos_list_index;
    ull select_map_for_lut_pos_list = 0;
    ull select_indx_for_lut_pos_list = 0;
    ull generated[410];
    const ull mask_lut_pos_indxsize =
        (table_mask << countBitsSet(num_mapas_mask)) | num_mapas_mask;
    ull number_of_generated_numbers = 0;
    while (remaining_bytes > 0) {
        random_select_coupled_chaotic_map_lookuptable_horizontal_perturbation(
            &position, roulete, num_mapas_mask, table_mask, Xn,
            mask_lut_pos_indxsize, parametros, lambda, epsilon, &H, generated,
            405, &number_of_generated_numbers,
            Xn[number_of_generated_numbers & mask_lut_pos_indxsize] & 0xFF);
        const ull chunk_size = sizeof(ull) * number_of_generated_numbers;
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;
        memcpy(rng_generated_buffer_write, generated, chunk_bytes);
        rng_generated_buffer_write += chunk_bytes;
        remaining_bytes -= chunk_bytes;
    }
#ifndef MEASURE_TIME_ONLY
    fwrite(rng_generated_buffer, sizeof(unsigned char), config->file_size,
           file);
    fclose(file);
#endif

    free(rng_generated_buffer);
}

int main() {
    const Configuracion config = readConfigFile("config.txt");
    const unsigned bitarray_size = 10;
    const unsigned bitarray_size_in_bits = bitarray_size * 8;
    char bitarray[bitarray_size];
    memset(bitarray, 0, bitarray_size);
    readBitArrayFromFile("rng_selector", bitarray, bitarray_size_in_bits);
    printf("%d\n", bitarray[0]);
    printf("%d\n", bitarray[1]);

    void (*generator[])(const char *file_path, const Configuracion *config) = {
        generate_random_file_1,  generate_random_file_2,
        generate_random_file_3,  generate_random_file_4,
        generate_random_file_5,  generate_random_file_6,
        generate_random_file_7,  generate_random_file_8,
        generate_random_file_9,  generate_random_file_10,
        generate_random_file_11, generate_random_file_12,
        generate_random_file_13, generate_random_file_14,
        generate_random_file_15,
    };

    char name[20];
    const char prefix[] = "random_data";
    char number[10];
    const char sufix[] = ".bin";
    const unsigned num_rng = sizeof(generator) / 8;

    memset(name, '\0', 20);

    for (unsigned i = 0; i < num_rng; ++i) {
        sprintf(number, "%u", i + 1);
        strcpy(name, prefix);
        strcat(name, number);
        strcat(name, sufix);
        if (ON(bitarray, i)) {
            printf("%s:\n\t", name);
            print_time({ generator[i](name, &config); });
        }
    }
    return 0;
}
