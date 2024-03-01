#include <arpa/inet.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "ConfigStructure.h"
#include "cipher.h"
#include "minmax.h"
#include "roulete.h"
#include "rtp_packet.h"

int create_udp_socket(uint16_t port, struct sockaddr_in *sockaddr) {
    memset(sockaddr, 0, sizeof(struct sockaddr_in));

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("failed to create socket");
        exit(EXIT_FAILURE);
    }

    (*sockaddr).sin_family = AF_INET;
    (*sockaddr).sin_port = htons(port);
    (*sockaddr).sin_addr.s_addr = INADDR_ANY;

    return sockfd;
}

int main() {
    // inicializar constantes
    const size_t payload_size = 1023;
    const size_t header_size = sizeof(struct rtp_header);
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

    // Cargar generador de numeros aleatorios
    Configuracion config;
    readConfigFile("config.txt", &config);
    uint8_t *random_buffer = malloc(config.file_size);
    struct rouleteConfig roulete_config;
    initilizale_roulete(&config, &roulete_config);
    roulete_generator(random_buffer, &config, &roulete_config);

    // inicializar socket
    struct sockaddr_in servaddr;
    int sockfd = create_udp_socket(12345, &servaddr);

    struct rtp_header header = {0};
    header.version = 0b10;
    header.padding = 1;
    header.extension = 0;
    header.seq_number = 0;
    header.timestamp = time(0);

    uint8_t hashindex = 0;
    uint8_t payload[payload_size + 1];
    uint8_t send_buffer[payload_size + header_size + 1];
    size_t index;
    uint8_t feedback;

    while (remaining) {
        const size_t sending_size = min(payload_size + 1, remaining + 1);
        if (fgets(payload, payload_size, stream) == NULL) {
            perror("Error while reading");
            goto cleanup;
        }
        // add hash to check integrity
        payload[payload_size] = hash[header.seq_number & index_mask];

        cipher(payload, random_buffer, sending_size, config.file_size,
               header.seq_number, &feedback);

        copy_rtp_packet(send_buffer, payload, sending_size, &header);

        if (sendto(sockfd, (const char *)send_buffer,
                   sending_size + header_size, 0,
                   (const struct sockaddr *)&servaddr,
                   sizeof(servaddr)) == -1) {
            perror("failed to send");
        }
        remaining -= sending_size - 1;
        header.seq_number++;
    }

cleanup:
    fclose(stream);
    close(sockfd);

    return 0;
}