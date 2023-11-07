#include <openssl/evp.h>
#include <stdio.h>
#include <stdlib.h>

#include "ConfigStructure.h"
#include "fn.c"
#include "time_measure.h"

#define SAMPLE_SIZE 1
#define MAX_ULL 0xFFFFFFFFFFFFFFFF
void calcular_hash(const unsigned char *data, size_t data_length,
                   int output_length, unsigned char *hash_value) {
    EVP_MD_CTX *mdctx;
    const EVP_MD *md;

    mdctx = EVP_MD_CTX_new();
    md = EVP_sha3_512();

    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, data, data_length);
    EVP_DigestFinal_ex(mdctx, hash_value, NULL);

    EVP_MD_CTX_free(mdctx);
}

void generate_random_file_1(const char *file_path,
                            const Configuracion *config) {
    typedef unsigned long long ull;
    FILE *file = fopen(file_path, "wb");
    if (file == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }

    ull X[NUMBER_OF_CAHOTIC_MAPS];
    ull Yi;
    calcular_hash((unsigned char *)&(config->seed), sizeof(ull),
                  sizeof(ull) * NUMBER_OF_CAHOTIC_MAPS, (unsigned char *)X);

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

    ull X[NUMBER_OF_CAHOTIC_MAPS];
    calcular_hash((unsigned char *)&(config->seed), sizeof(ull),
                  sizeof(ull) * NUMBER_OF_CAHOTIC_MAPS, (unsigned char *)X);
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

    ull X[NUMBER_OF_CAHOTIC_MAPS];
    calcular_hash((unsigned char *)&(config->seed), sizeof(ull),
                  sizeof(ull) * NUMBER_OF_CAHOTIC_MAPS, (unsigned char *)X);
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
    X = (X<<32)|rand();

    const size_t chunk_size =
        sizeof(ull);  // Tamaño de cada fragmento a escribir
    size_t remaining_bytes = config->file_size;
    ull divisor = config->r;
    ull t = (0xFFFFFFFFFFFFFFFF / divisor);
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
    X = (X<<32)|rand();
    ull Yi[255];

    ull size = config->n;

    const size_t chunk_size =
        sizeof(ull) * size;  // Tamaño de cada fragmento a escribir
    size_t remaining_bytes = config->file_size;
    ull t = (0xFFFFFFFFFFFFFFFF / config->r);
    t = sqrtull(t);
    while (remaining_bytes > 0) {
        // Generar datos aleatorios para el fragmento actual
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;
        X = logistic_renyi_with_cycle(X, Yi, config->beta, config->lambda, config->r, t, size);

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
    X = (X<<32)|rand();
    ull Yi[255];

    ull mask = 0b1111;

    ull divisor = config->r;
    size_t remaining_bytes = config->file_size;
    ull t = (0xFFFFFFFFFFFFFFFF / divisor);
    t = sqrtull(t);
    while (remaining_bytes > 0) {
        const ull size = (X & mask)| 1; 
    const size_t chunk_size =
        sizeof(ull) * size;  // Tamaño de cada fragmento a escribir
        // Generar datos aleatorios para el fragmento actual
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;
        X = logistic_renyi_with_random_cycle(X, Yi, config->beta, config->lambda, config->r, t, mask);

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
    X = (X<<32)|rand();

    const size_t chunk_size =
        sizeof(ull);  // Tamaño de cada fragmento a escribir
    size_t remaining_bytes = config->file_size;
    ull divisor = config->r;
    ull t = (0xFFFFFFFFFFFFFFFF / divisor);
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



int main() {
    const Configuracion config = readConfigFile("config.txt");
    printf("%llu\n", config.file_size);
    print_time({
        const char *file_path = "random_data1.bin";
        generate_random_file_1(file_path, &config);
    });
    print_time({
        const char *file_path = "random_data2.bin";
        generate_random_file_2(file_path, &config);
    });
    print_time({
        const char *file_path = "random_data3.bin";
        generate_random_file_3(file_path, &config);
    });
    print_time({
        const char *file_path = "random_data4.bin";
        generate_random_file_4(file_path, &config);
    });
    print_time({
        const char *file_path = "random_data5.bin";
        generate_random_file_5(file_path, &config);
    });
    print_time({
        const char *file_path = "random_data6.bin";
        generate_random_file_6(file_path, &config);
    });
    print_time({
        const char *file_path = "random_data7.bin";
        generate_random_file_7(file_path, &config);
    });
    return 0;
}