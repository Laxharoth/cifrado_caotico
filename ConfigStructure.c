#include "ConfigStructure.h"

Configuracion readConfigFile(const char *filename) {
    Configuracion config;
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("No se pudo abrir el archivo de configuración.\n");
        return config;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        // Omitir líneas en blanco y comentarios
        if (line[0] == '\n' || line[0] == '#') {
            continue;
        }

        // Buscar el delimitador de clave y valor (=)
        char *delimiter = strchr(line, '=');
        if (delimiter == NULL) {
            continue;
        }

        // Dividir la línea en clave y valor
        *delimiter =
            '\0';  // Reemplazar el delimitador por un terminador de cadena
        char *key = line;
        char *value = delimiter + 1;
        char *endptr;

        // Eliminar espacios en blanco alrededor de la clave y el valor
        char *end = value + strlen(value) - 1;
        while (end > value && (*end == '\n' || *end == ' ' || *end == '\t')) {
            *end-- = '\0';
        }
        while (*key == ' ' || *key == '\t') {
            key++;
        }

        // Guardar la clave y el valor en la estructura
        if (strcmp(key, "beta") == 0) {
            config.beta = atoi(value);
        } else if (strcmp(key, "lambda") == 0) {
            config.lambda = atoi(value);
        } else if (strcmp(key, "r") == 0) {
            config.file_size = atoi(value);
        } else if (strcmp(key, "seed") == 0) {
            config.seed = strtoull(value, &endptr, 10);
        } else if (strcmp(key, "file_size") == 0) {
            config.file_size = strtoull(value, &endptr, 10);
        }
    }

    fclose(file);

    return config;
}