#include "cipher.h"

void cipher(u_int8_t *plain_text, u_int8_t *const cipher_stream,
            u_int64_t length, u_int64_t cipher_stream_size,
            u_int64_t cipher_stream_index, u_int8_t *feedback) {
    *feedback = 0;
    for (u_int64_t i = 0; i < length; ++i, ++plain_text) {
        const u_int64_t cipher_stream_current =
            (cipher_stream_index + i) % cipher_stream_size;
        *feedback ^= cipher_stream[cipher_stream_current];
        *plain_text ^= *feedback;
    }
    *feedback ^= *cipher_stream;
}

void refill_cipher_stream(u_int8_t *const cipher_stream, u_int64_t amount,
                          u_int64_t cipher_stream_size,
                          u_int64_t cipher_stream_index, generator gen) {
    for (u_int64_t i = 0; i < amount; ++i) {
        const u_int64_t cipher_stream_current =
            (cipher_stream_index + i) % cipher_stream_size;
        cipher_stream[cipher_stream_current] = gen();
    }
}