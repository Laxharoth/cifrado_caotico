#ifndef __CIPHER_H__
#define __CIPHER_H__

#include <stdint.h>

#include "ConfigStructure.h"

typedef uint64_t (*generator)();

void cipher(uint64_t *plain_text, uint64_t *const cipher_stream,
            uint64_t length, uint64_t cipher_stream_size,
            uint64_t cipher_stream_index, uint8_t *hash_ref);

void decipher(uint64_t *plain_text, uint64_t *const cipher_stream,
              uint64_t length, uint64_t cipher_stream_size,
              uint64_t cipher_stream_index, uint8_t *hash_ref);

void refill_cipher_stream(uint64_t *const cipher_stream, uint64_t amount,
                          uint64_t cipher_stream_size,
                          uint64_t cipher_stream_index, generator gen);
#endif  //__CIPHER_H