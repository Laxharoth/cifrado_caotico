#include <arpa/inet.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "ConfigStructure.h"
#include "cipher.h"
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

struct rouleteConfig *global_roulete;

uint8_t random_generator() {
    static uint8_t index = 8;
    static uint64_t value;
    if (index >= 8) {
        index = 0;
        value = random_select_coupled_chaotic_map_lookuptable(global_roulete);
    }
    return ((uint8_t *)&value)[index++];
}

int main() {
    const size_t payload_size = 1024;
    const size_t header_size = sizeof(struct rtp_header);

    uint8_t buffer[payload_size + header_size + 1];
    struct rtp_header *const header = (struct rtp_header *)buffer;
    uint8_t *const payload = buffer + header_size;

    struct sockaddr_in servaddr = {0};

    int sockfd = create_udp_socket(12345, &servaddr);

    int rc = bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr));

    if (rc == -1) {
        perror("failed to bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    socklen_t len = 0;
    size_t last_decipher = 0;

    const uint8_t hash[] = {
        114, 241, 40,  167, 67,  66,  146, 166, 113, 109, 92,  94, 125,
        245, 54,  143, 91,  210, 107, 183, 229, 219, 84,  54,  49, 173,
        152, 202, 165, 166, 252, 57,  200, 30,  168, 7,   41,  3,  90,
        214, 228, 220, 39,  58,  233, 218, 116, 84,  28,  115, 34, 168,
        111, 107, 75,  137, 208, 133, 130, 98,  24,  183, 231, 99};
    const uint8_t index_mask = 0b111111;

    // Cargar generador de numeros aleatorios
    Configuracion config;
    readConfigFile("config.txt", &config);
    global_roulete = malloc(sizeof(struct rouleteConfig));
    initilizale_roulete(&config, global_roulete);

    uint8_t *random_buffer = malloc(config.file_size);
    roulete_generator(random_buffer, &config, global_roulete);

    uint8_t feedback;

    while (1) {
        int n = recvfrom(sockfd, (char *)buffer, payload_size + header_size,
                         MSG_WAITALL, 0, &len);
        cipher(payload, random_buffer, n, config.file_size, header->seq_number,
               &feedback);

        const uint8_t got_hash = buffer[n - 1];

        if (hash[header->seq_number & index_mask] != buffer[n - 1]) {
            printf("WARNING: HASH DOES NOT MATCH %u DOES NOT MATCH %u",
                   hash[header->seq_number & index_mask], got_hash);
        }

        printf("%06d bytes read,  sequence number: %06d\n", n,
               header->seq_number);
    }
    close(sockfd);

    free(global_roulete);
    return 0;
}