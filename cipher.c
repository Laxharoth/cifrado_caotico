#include "cipher.h"

void cipher(uint64_t *plain_text, uint64_t *const cipher_stream,
            uint64_t length, uint64_t cipher_stream_size,
            uint64_t cipher_stream_index, uint8_t *hash_ref) {
    uint64_t sum = 0;
    uint64_t iter;
    uint8_t *sum_bytes = (uint8_t *)&sum;
    for (iter = 0; iter < length; ++iter, ++plain_text) {
        const uint64_t cipher_stream_current_index =
            (cipher_stream_index + iter) % cipher_stream_size;
        sum += *plain_text;
        *plain_text ^= cipher_stream[cipher_stream_current_index];
    }
    for (iter = 0; iter < sizeof(uint64_t); iter++, sum_bytes++) {
        *hash_ref ^= *sum_bytes;
    }
}

void decipher(uint64_t *plain_text, uint64_t *const cipher_stream,
              uint64_t length, uint64_t cipher_stream_size,
              uint64_t cipher_stream_index, uint8_t *hash_ref) {
    uint64_t sum = 0;
    uint64_t iter;
    uint8_t *sum_bytes = (uint8_t *)&sum;
    for (iter = 0; iter < length; ++iter, ++plain_text) {
        const uint64_t cipher_stream_current_index =
            (cipher_stream_index + iter) % cipher_stream_size;
        *plain_text ^= cipher_stream[cipher_stream_current_index];
        sum += *plain_text;
    }
    for (iter = 0; iter < sizeof(uint64_t); iter++, sum_bytes++) {
        *hash_ref ^= *sum_bytes;
    }
}

void refill_cipher_stream(uint64_t *const cipher_stream, uint64_t amount,
                          uint64_t cipher_stream_size,
                          uint64_t cipher_stream_index, generator gen) {
    for (uint64_t i = 0; i < amount; ++i) {
        const uint64_t cipher_stream_current =
            (cipher_stream_index + i) % cipher_stream_size;
        cipher_stream[cipher_stream_current] = gen();
    }
}