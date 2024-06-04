#include <omp.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SAMPLE_SIZE 10

#include "ConfigStructure.h"
#include "bitarray.h"
#include "roulete.h"

void hamming_distance(uint64_t *const a, uint64_t *const b, const uint64_t size,
                      uint64_t *const distance) {
    uint64_t sum = 0;
    uint64_t i, j;
    // #pragma omp parallel for reduce(+ : sum)
    for (i = 0; i < size; i++) {
        const uint64_t diff = a[i] ^ b[i];
        for (j = 0; j < 64ull; j++) sum += (diff >> j) & 1;
    }
    *distance = sum;
}
int main() {
    // Cargar generador de números aleatorios
    Configuracion config;
    readConfigFile("config.txt", &config);
    // sobreescribir el tamaño del archivo por el numero deseado de numeros
    // aleatorios
    const size_t precalculated_packets = 32;
    uint64_t *const random_buffer = (uint64_t *)malloc(config.file_size);
    uint64_t *const random_buffer2 = (uint64_t *)malloc(config.file_size);
    struct rouleteConfig roulete_config;
    struct rouleteConfig roulete_config2;
    size_t total_checks = config.num_maps * sizeof(uint64_t) * 8;
    double total_distance = 0;
    size_t counter = 1;
    for (size_t i = 0; i < config.num_maps; i++) {
        for (size_t j = 0; j < sizeof(uint64_t) * 8; j++) {
            initilizale_roulete(&config, &roulete_config);
            initilizale_roulete(&config, &roulete_config2);

            roulete_config2.roulete[i].parameter_r ^= 1 << j;

            roulete_generator(random_buffer, &config, &roulete_config);
            roulete_generator(random_buffer2, &config, &roulete_config2);
            uint64_t distance;
            hamming_distance(random_buffer, random_buffer2,
                             config.file_size / 8, &distance);
            total_distance += distance;
            printf("distance %05d/%05d: %f\n", counter, total_checks,
                   (double)(distance) / (double)(config.file_size * 8));

#ifndef MEASURE_TIME_ONLY
            printf("\tGuardando archivo.\n");
            FILE *file = fopen("data/random_data_hming1.bin", "wb");
            if (file == NULL) {
                printf("No se pudo abrir el archivo.\n");
            }
            fwrite(random_buffer, sizeof(unsigned char), config.file_size,
                   file);
            fclose(file);
            file = fopen("data/random_data_hming2.bin", "wb");
            if (file == NULL) {
                printf("No se pudo abrir el archivo.\n");
            }
            fwrite(random_buffer2, sizeof(unsigned char), config.file_size,
                   file);
            fclose(file);
#endif

            delete_roulete(&roulete_config);
            delete_roulete(&roulete_config2);
            counter++;
        }
    }

    free(random_buffer);
    free(random_buffer2);
    printf("total distance: %f\n", (double)(total_distance) /
                                       (double)(config.file_size * 8) /
                                       total_checks);

    return 0;
}
