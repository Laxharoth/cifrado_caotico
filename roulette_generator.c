#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define SAMPLE_SIZE 1000

#include "ConfigStructure.c"
#include "ConfigStructure.h"
#include "cipher.c"
#include "cipher.h"
#include "minmax.h"
#include "roulete.c"
#include "roulete.h"

#define elif(condition) else if (condition)

int main() {
    // Cargar generador de números aleatorios
    Configuracion config;
    readConfigFile("config.txt", &config);
    // sobreescribir el tamaño del archivo por el numero deseado de números
    // aleatorios
    const size_t precalculated_packets = 32;
    uint64_t *const random_buffer = malloc(config.file_size);
    struct rouleteConfig roulete_config;
    initilizale_roulete(&config, &roulete_config);
    roulete_generator(random_buffer, &config, &roulete_config);

    FILE *out = fopen("random_datar.bin", "wb");

    fwrite(random_buffer, config.file_size, 1, out);

    free(random_buffer);

    return 0;
}