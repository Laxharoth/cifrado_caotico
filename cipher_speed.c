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
#include "rtp_packet.c"
#include "rtp_packet.h"
#include "time_measure.h"

#define elif(condition) else if (condition)

int main() {
    // inicializar constantes
    const size_t payload_size = 1024;
    const size_t payload_size_64 = payload_size / sizeof(uint64_t);
    const size_t required_random_numbers = payload_size_64 * 2 + 1;
    const size_t header_size = sizeof(struct rtp_header);
    const uint8_t index_mask = 0b111111;
    // const size_t stream_size = 1073741824;  // 1 GB
    const size_t stream_size = 1048576;  // 1 MB

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
    uint8_t *const stream_start = malloc(stream_size);

    // Cargar generador de números aleatorios
    Configuracion config;
    readConfigFile("config.txt", &config);
    // sobreescribir el tamaño del archivo por el numero deseado de números
    // aleatorios
    const size_t precalculated_packets = 32;
    const size_t random_buffer_size_64 =
        required_random_numbers * precalculated_packets;
    config.file_size = stream_size;  // precalcular 20 paquetes
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

    double time_secs = time_exec(        // comienza ejecucion de cifrado
        size_t remaining = stream_size;  // inicializa el tamaño de cifrado
        uint8_t *stream = stream_start; while (remaining) {
            const size_t sending_size = min(payload_size, remaining);
            if (memcpy((char *)payload, stream, payload_size) == NULL) {
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
            // Refill cipher stream
            const size_t packets_to_refill = header->seq_number - last_decipher;
            for (uint64_t pack = 0; pack < packets_to_refill; ++pack) {
                const size_t initial_index =
                    ((last_decipher + pack) % precalculated_packets) *
                    required_random_numbers;
                const uint64_t *end =
                    random_buffer + initial_index + required_random_numbers;
                for (uint64_t *start = random_buffer + initial_index;
                     start < end; ++start) {
                    *start =
                        random_select_coupled_chaotic_map_lookuptable_bitoffset(
                            &roulete_config);
                }
            }
            last_decipher = header->seq_number;
            header->seq_number++;
            //
            stream += sending_size;
            remaining -= sending_size;
        });
    printf("Velocidad de cifrado: %0.4f Gbps\n",
           stream_size * 8 / time_secs / 1e9);

    free(random_buffer);
    free(stream_start);

    return 0;
}