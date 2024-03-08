#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "ConfigStructure.h"
#include "cipher.h"
#include "minmax.h"
#include "roulete.h"
#include "rtp_packet.h"

int main() {
    // inicializar constantes
    const size_t payload_size = 1024;
    const size_t payload_size_64 = payload_size / sizeof(uint64_t);
    const size_t header_size = sizeof(struct rtp_header);

    // Asegurarse de que tiene un tamaño compatible
    if (payload_size % sizeof(uint64_t) != 0) {
        // error message for wrong payload size
        printf("Error: payload size mismatch");
        exit(EXIT_FAILURE);
    }

    const uint8_t hash[] = {
        114, 241, 40,  167, 67,  66,  146, 166, 113, 109, 92,  94, 125,
        245, 54,  143, 91,  210, 107, 183, 229, 219, 84,  54,  49, 173,
        152, 202, 165, 166, 252, 57,  200, 30,  168, 7,   41,  3,  90,
        214, 228, 220, 39,  58,  233, 218, 116, 84,  28,  115, 34, 168,
        111, 107, 75,  137, 208, 133, 130, 98,  24,  183, 231, 99};
    const uint8_t index_mask = 0b111111;
    // Cargar data para enviar
    FILE *stream = fopen("random_data15.bin", "r");
    fseek(stream, 0, SEEK_END);
    const size_t stream_size = ftell(stream);
    size_t remaining = stream_size;
    printf("size %lu", stream_size);
    rewind(stream);

    // Cargar generador de números aleatorios
    Configuracion config;
    readConfigFile("config.txt", &config);
    uint64_t *random_buffer = malloc(config.file_size);
    struct rouleteConfig roulete_config;
    initilizale_roulete(&config, &roulete_config);
    roulete_generator(random_buffer, &config, &roulete_config);

    // set the initial address for header, payload, hash and feedback
    const uint64_t send_buffer_size = header_size + payload_size + 1;
    void *send_buffer = malloc(send_buffer_size);  // last byte is hash
    struct rtp_header *header = (struct rtp_header *)send_buffer;
    uint64_t *payload = send_buffer + sizeof(struct rtp_header);
    uint8_t *hash_ref = send_buffer + (header_size + payload_size);

    // inicializa la cabecera de rtp
    header->version = 0b10;
    header->padding = 1;
    header->extension = 0;
    header->seq_number = 0;
    header->timestamp = time(0);

    while (remaining) {
        const size_t sending_size = min(payload_size, remaining);
        if (fgets((char *)payload, payload_size, stream) == NULL) {
            perror("Error while reading");
            goto cleanup;
        }
        // padding
        memset(payload + sending_size, 0, payload_size - sending_size);
        // add hash to check integrity
        *hash_ref = hash[header->seq_number & index_mask];

        cipher(payload, random_buffer, payload_size_64, config.file_size,
               (header->seq_number) * payload_size_64, hash_ref);

        header->timestamp = time(0);

        decipher(payload, random_buffer, payload_size_64, config.file_size,
                 (header->seq_number) * payload_size_64, hash_ref);

        if (hash[header->seq_number & index_mask] != *hash_ref) {
            printf("WARNING: HASH DOES NOT MATCH %u DOES NOT MATCH %u",
                   hash[header->seq_number & index_mask], *hash_ref);
        }

        remaining -= sending_size;
        header->seq_number++;
    }

cleanup:
    free(random_buffer);
    free(send_buffer);
    fclose(stream);

    return 0;
}