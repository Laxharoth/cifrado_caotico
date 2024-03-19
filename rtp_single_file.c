#include <stddef.h>
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
#include "time_measure.h"

#define elif(condition) else if (condition)

int main() {
    // inicializar constantes
    const size_t payload_size = 1024;
    const size_t payload_size_64 = payload_size / sizeof(uint64_t);
    const size_t required_random_numbers = payload_size_64 * 2 + 1;
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
    rewind(stream);

    // Cargar generador de números aleatorios
    Configuracion config;
    readConfigFile("config.txt", &config);
    // sobreescribir el tamaño del archivo por el numero deseado de numeros
    // aleatorios
    const size_t precalculated_packets = 32;
    const size_t random_buffer_size_64 =
        required_random_numbers * precalculated_packets;
    config.file_size =
        sizeof(uint64_t) * random_buffer_size_64;  // precalcular 20 paquetes
    uint64_t *const random_buffer = malloc(config.file_size);
    struct rouleteConfig roulete_config;
    initilizale_roulete(&config, &roulete_config);
    roulete_generator(random_buffer, &config, &roulete_config);

    uint64_t aux_renyi_r = config.r;
    uint64_t aux_renyi_j = config.lambda;

    // set the initial address for header, payload, hash and feedback
    const uint64_t send_buffer_size = header_size + payload_size + 1;
    void *const send_buffer = malloc(send_buffer_size);  // last byte is hash
    struct rtp_header *header = (struct rtp_header *)send_buffer;
    uint64_t *payload = send_buffer + sizeof(struct rtp_header);
    uint8_t *hash_ref = send_buffer + (header_size + payload_size);

    // inicializa la cabecera de rtp
    header->version = 0b10;
    header->padding = 1;
    header->extension = 0;
    header->seq_number = 0;
    header->timestamp = time(0);

    unsigned last_decipher = 0;

    print_time(

        while (remaining) {
            const size_t sending_size = min(payload_size, remaining);
            if (fgets((char *)payload, payload_size, stream) == NULL) {
                perror("Error while reading");
                exit(EXIT_FAILURE);
            }
            // padding
            memset(payload + sending_size, 0, payload_size - sending_size);
            // add hash to check integrity
            *hash_ref = hash[header->seq_number & index_mask];
            cipher(payload, random_buffer, payload_size_64,
                   random_buffer_size_64,
                   (header->seq_number) * payload_size_64, hash_ref,
                   aux_renyi_r, aux_renyi_j);

            header->timestamp = time(0);

            decipher(payload, random_buffer, payload_size_64,
                     random_buffer_size_64,
                     (header->seq_number) * payload_size_64, hash_ref,
                     aux_renyi_r, aux_renyi_j);

            if (hash[header->seq_number & index_mask] != *hash_ref) {
                printf(
                    "WARNING: HASH DOES NOT MATCH EXPECTED \"%u\" DOES NOT "
                    "MATCH \"%u\"",
                    hash[header->seq_number & index_mask], *hash_ref);
                exit(EXIT_FAILURE);
            }
            elif (header->seq_number < last_decipher) {
                printf(
                    "WARNING: CURRENT SEQUENCE NUMBER %u IS LOWER THAN LAST "
                    "DECIPHER %u",
                    header->seq_number, last_decipher);
            }
            else {
                const size_t packets_to_refill =
                    header->seq_number - last_decipher;
                const size_t initial_index =
                    (last_decipher % precalculated_packets) *
                    required_random_numbers;
                for (uint64_t i = 0;
                     i < packets_to_refill * required_random_numbers; ++i) {
                    const uint64_t cipher_stream_current =
                        (initial_index + i) % random_buffer_size_64;
                    random_buffer[cipher_stream_current] =
                        random_select_coupled_chaotic_map_lookuptable(
                            &roulete_config);
                }
                last_decipher = header->seq_number;
            }

            remaining -= sending_size;
            header->seq_number++;
        })

        free(random_buffer);
    fclose(stream);

    return 0;
}