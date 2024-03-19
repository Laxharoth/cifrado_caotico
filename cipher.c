#include "cipher.h"

void cipher(uint64_t *plain_text, uint64_t *const cipher_stream,
            uint64_t length, uint64_t cipher_stream_size,
            uint64_t cipher_stream_index, uint8_t *hash_ref,
            uint64_t aux_renyi_r, uint64_t aux_renyi_j) {
    uint64_t sum = 0;
    uint64_t iter;
    uint64_t cipher_text_ant = 0;
    uint8_t *sum_bytes = (uint8_t *)&sum;
    for (iter = 0; iter < length; iter += 2, ++plain_text) {
        const uint64_t cipher_stream_current_index =
            (cipher_stream_index + iter) % cipher_stream_size;
        const uint64_t cipher_stream_current_index_next =
            (cipher_stream_index + iter + 1) % cipher_stream_size;
        const uint64_t num_1 = cipher_stream[cipher_stream_current_index];
        const uint64_t num_2 = cipher_stream[cipher_stream_current_index_next];
        const uint64_t renyi_cipher_val =
            RenyiMap(cipher_text_ant, aux_renyi_r, aux_renyi_j);

        sum += *plain_text;
        *plain_text = (*plain_text + num_1) ^ num_2 ^ renyi_cipher_val;
        cipher_text_ant = *plain_text;
    }
    for (iter = 0; iter < sizeof(uint64_t); iter++, sum_bytes++) {
        *hash_ref ^= *sum_bytes;
    }
}

void decipher(uint64_t *cipher_text, uint64_t *const cipher_stream,
              uint64_t length, uint64_t cipher_stream_size,
              uint64_t cipher_stream_index, uint8_t *hash_ref,
              uint64_t aux_renyi_r, uint64_t aux_renyi_j) {
    uint64_t sum = 0;
    uint64_t iter;
    uint64_t cipher_text_ant = 0;
    uint8_t *sum_bytes = (uint8_t *)&sum;
    for (iter = 0; iter < length; iter += 2, ++cipher_text) {
        const uint64_t cipher_stream_current_index =
            (cipher_stream_index + iter) % cipher_stream_size;
        const uint64_t cipher_stream_current_index_next =
            (cipher_stream_index + iter + 1) % cipher_stream_size;
        const uint64_t num_1 = cipher_stream[cipher_stream_current_index];
        const uint64_t num_2 = cipher_stream[cipher_stream_current_index_next];
        const uint64_t renyi_cipher_val =
            RenyiMap(cipher_text_ant, aux_renyi_r, aux_renyi_j);

        cipher_text_ant = *cipher_text;
        *cipher_text = (*cipher_text ^ num_2 ^ renyi_cipher_val) - num_1;
        sum += *cipher_text;
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