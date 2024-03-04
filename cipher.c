#include "cipher.h"

void cipher(uint8_t *plain_text, uint8_t *const cipher_stream, uint64_t length,
            uint64_t cipher_stream_size, uint64_t cipher_stream_index,
            uint8_t *feedback) {
    *feedback = 0;
    for (uint64_t i = 0; i < length; ++i, ++plain_text) {
        const uint64_t cipher_stream_current =
            (cipher_stream_index + i) % cipher_stream_size;
        *feedback ^= cipher_stream[cipher_stream_current];
        *plain_text ^= *feedback;
    }
}

void refill_cipher_stream(uint8_t *const cipher_stream, uint64_t amount,
                          uint64_t cipher_stream_size,
                          uint64_t cipher_stream_index, generator gen) {
    for (uint64_t i = 0; i < amount; ++i) {
        const uint64_t cipher_stream_current =
            (cipher_stream_index + i) % cipher_stream_size;
        cipher_stream[cipher_stream_current] = gen();
    }
}