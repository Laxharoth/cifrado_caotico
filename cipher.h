#ifndef __CIPHER_H__
#define __CIPHER_H__

#include <stdint.h>

#include "ConfigStructure.h"
#include "chaotic_map.h"

typedef uint64_t (*generator)();

void cipher(uint64_t *plain_text, uint64_t *const cipher_stream,
            uint64_t length, uint64_t cipher_stream_size,
            uint64_t cipher_stream_index, uint8_t *hash_ref,
            uint64_t aux_renyi_r, uint64_t aux_renyi_j);

void decipher(uint64_t *cipher_text, uint64_t *const cipher_stream,
              uint64_t length, uint64_t cipher_stream_size,
              uint64_t cipher_stream_index, uint8_t *hash_ref,
              uint64_t aux_renyi_r, uint64_t aux_renyi_j);

void refill_cipher_stream(uint64_t *const cipher_stream, uint64_t amount,
                          uint64_t cipher_stream_size,
                          uint64_t cipher_stream_index, generator gen);
#endif  //__CIPHER_H