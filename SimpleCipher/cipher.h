#ifndef CIPHER_H
#define CIPHER_H
#include <stdint.h>
#include <stdlib.h>

void cipher(uint64_t *plaintext, uint64_t *cipher_stream,
            uint64_t plaintext_len);
#endif /* CIPHER_H */
