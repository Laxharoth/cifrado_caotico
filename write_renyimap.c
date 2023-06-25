#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

#define RenyiMap(X, beta, lambda) (X * beta) + (X >> lambda)
typedef unsigned long long ull;

void generate_random_file(const char* file_path, size_t file_size) {
    FILE* file = fopen(file_path, "wb");
    if (file == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }

    const size_t chunk_size =
        sizeof(ull);  // Tamaño de cada fragmento a escribir
    size_t remaining_bytes = file_size;

    ull X0 = 100;
    ull Xi = X0, beta1 = 15, beta2 = 3, beta3 = 5, beta4 = 7, lambda = 3;
    while (remaining_bytes > 0) {
        // Generar datos aleatorios para el fragmento actual
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;

        Xi =  ((RenyiMap((Xi), beta1, lambda)) >> 2)
             +((RenyiMap((Xi), beta2, lambda)) >> 2)
             +((RenyiMap((Xi), beta3, lambda)) >> 2)
             +((RenyiMap((Xi), beta4, lambda)) >> 2);

        fwrite(&Xi, sizeof(unsigned char), chunk_bytes, file);
        remaining_bytes -= chunk_bytes;
    }

    fclose(file);
}

int main() {
    const char* file_path =
        "random_data.bin";  // Ruta donde se guardará el archivo
    size_t file_size =
        1024 * 1024;  // Tamaño del archivo en bytes (por ejemplo, 1 MB)

    srand(time(
        NULL));  // Inicializar la semilla de generación de números aleatorios

    generate_random_file(file_path, file_size);

    return 0;
}