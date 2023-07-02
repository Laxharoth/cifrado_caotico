#include <openssl/evp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

#include "ConfigStructure.h"
#include "fn.c"

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
        // Yi = renyi_array_generator(X);

        fwrite(&Yi, sizeof(unsigned char), chunk_bytes, file);
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

        fwrite(&Yi, sizeof(unsigned char), chunk_bytes, file);
        remaining_bytes -= chunk_bytes;
    }

    fclose(file);
}

void generate_random_file(const char *file_path, const Configuracion *config) {
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
        sizeof(ull) * NUMBER_OF_CAHOTIC_MAPS;  // Tamaño de cada fragmento a escribir
    size_t remaining_bytes = config->file_size;
    while (remaining_bytes > 0) {
        // Generar datos aleatorios para el fragmento actual
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;

        renyi_array_generator(X, config->beta, config->lambda);
        // Yi = renyi_array_generator(X);

        fwrite(&X, sizeof(unsigned char), chunk_bytes, file);
        remaining_bytes -= chunk_bytes;
    }

    fclose(file);
}

int main() {
    const Configuracion config = readConfigFile("config.txt");
    printf("%llu\n", config.file_size);

    {
        const char *file_path = "random_data.bin";
        generate_random_file(file_path, &config);
    }
    {
        const char *file_path = "random_data2.bin";
        generate_random_file_2(file_path, &config);
    }
    {
        const char *file_path = "random_data3.bin";
        generate_random_file_3(file_path, &config);
    }
    return 0;
}