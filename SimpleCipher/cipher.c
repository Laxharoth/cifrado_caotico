#include "cipher.h"

void xor_cipher(uint64_t *plaintext, uint64_t *cipher_stream,
                uint64_t plaintext_len) {
    for (uint64_t i = 0; i < plaintext_len; i++) {
        plaintext[i] = cipher_stream[i] ^ cipher_stream[i];
    }
}