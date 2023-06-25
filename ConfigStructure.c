#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 100

typedef struct {
    int lambda;
    int beta1;
} Configuracion;

void readConfigFile(const char *filename, Configuracion *config) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("No se pudo abrir el archivo de configuración.\n");
        return;
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

        // Eliminar espacios en blanco alrededor de la clave y el valor
        char *end = value + strlen(value) - 1;
        while (end > value && (*end == '\n' || *end == ' ' || *end == '\t')) {
            *end-- = '\0';
        }
        while (*key == ' ' || *key == '\t') {
            key++;
        }

        // Guardar la clave y el valor en la estructura
        if (strcmp(key, "lambda") == 0) {
            config->lambda = atoi(value);
        } else if (strcmp(key, "beta1") == 0) {
            config->beta1 = atoi(value);
        }
    }

    fclose(file);
}