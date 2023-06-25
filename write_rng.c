#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

#include "fn.c"

void generate_random_file(const char* file_path, size_t file_size) {
    typedef unsigned long long ull ;
    FILE* file = fopen(file_path, "wb");
    if (file == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }
    const ull SEED = 0xAC2ED40CAC2ED40C;
    unsigned char* seed = (unsigned char*)(&SEED);
    ull X[8];
    unsigned char* x = (unsigned char*)(void*)X;
    ull Yi;
    SHA512(seed, sizeof(ull), x);

    const size_t chunk_size =
        sizeof(ull);  // Tamaño de cada fragmento a escribir
    size_t remaining_bytes = file_size;
    while (remaining_bytes > 0) {
        // Generar datos aleatorios para el fragmento actual
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;

        Yi = renyi_array_2(X);
        // Yi = renyi_array_generator(X);

        fwrite(&Yi, sizeof(unsigned char), chunk_bytes, file);
        remaining_bytes -= chunk_bytes;
    }

    fclose(file);
}

int main() {
    const char* file_path =
        "random_data.bin";  // Ruta donde se guardará el archivo
    size_t file_size =
        1024 * 1024 * 50;  // Tamaño del archivo en bytes (por ejemplo, 1 MB)

    srand(time(
        NULL));  // Inicializar la semilla de generación de números aleatorios

    generate_random_file(file_path, file_size);

    return 0;
}