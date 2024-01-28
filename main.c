#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ConfigStructure.h"
#include "bitarray.c"
#include "generators.c"
#include "time_measure.h"

#define SAMPLE_SIZE 1
#define MAX_ULL 0xFFFFFFFFFFFFFFFF

int main() {
    Configuracion config;
    readConfigFile("config.txt", &config);
    const unsigned bitarray_size = 10;
    const unsigned bitarray_size_in_bits = bitarray_size * 8;
    char bitarray[bitarray_size];
    memset(bitarray, 0, bitarray_size);
    readBitArrayFromFile("rng_selector", bitarray, bitarray_size_in_bits);
    printf("%d\n", bitarray[0]);
    printf("%d\n", bitarray[1]);

    unsigned char *buffer = malloc(config.file_size);

    void (*generator[])(unsigned char *const buffer,
                        const Configuracion *config) = {
        generate_random_file_1,  generate_random_file_2,
        generate_random_file_3,  generate_random_file_4,
        generate_random_file_5,  generate_random_file_6,
        generate_random_file_7,  generate_random_file_8,
        generate_random_file_9,  generate_random_file_10,
        generate_random_file_11, generate_random_file_12,
        generate_random_file_13, generate_random_file_14,
        generate_random_file_15,
    };

    char file_path[20];
    const char prefix[] = "random_data";
    char number[10];
    const char sufix[] = ".bin";
    const unsigned num_rng = sizeof(generator) / 8;

    memset(file_path, '\0', 20);

    for (unsigned i = 0; i < num_rng; ++i) {
        sprintf(number, "%u", i + 1);
        strcpy(file_path, prefix);
        strcat(file_path, number);
        strcat(file_path, sufix);
        if (ON(bitarray, i)) {
            srand(config.seed);
            printf("%s:\n\t", file_path);
            print_time({ generator[i](buffer, &config); });
#ifndef MEASURE_TIME_ONLY
            printf("\tGuardando archivo.\n");
            FILE *file = fopen(file_path, "wb");
            if (file == NULL) {
                printf("No se pudo abrir el archivo.\n");
                continue;
            }
            fwrite(buffer, sizeof(unsigned char), config.file_size, file);
            fclose(file);
#endif
        }
    }
    free(buffer);
    return 0;
}
