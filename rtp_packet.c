#include "rtp_packet.h"

#include <string.h>

void copy_rtp_packet(uint8_t *dest, uint8_t *payload, uint64_t payload_size,
                     struct rtp_header *header) {
    memcpy(dest, header, sizeof(struct rtp_header));
    memcpy(dest + sizeof(struct rtp_header), payload, payload_size);
}