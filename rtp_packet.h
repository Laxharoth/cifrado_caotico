#ifndef RTP_PACKET_H
#define RTP_PACKET_H
#include <stdint.h>

typedef struct rtp_header {
    uint16_t version : 2;       // Version
    uint16_t padding : 1;       // Padding bit
    uint16_t extension : 1;     // Extension bit
    uint16_t contribution : 4;  // Contribution bit
    uint16_t marker : 1;        // Marker bit
    uint16_t payload_type : 7;  // Payload type
    uint16_t seq_number;        // Sequence number
    uint32_t timestamp;         // Timestamp
    uint32_t ssrc;              // Synchronization source
} rtp_header_t;

void copy_rtp_packet(uint8_t *dest, uint8_t *payload, uint64_t payload_size,
                     struct rtp_header *header);

#endif  // RTP_PACKET_H
