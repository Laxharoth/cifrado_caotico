#ifndef __CIPHER_H__
#define __CIPHER_H__

#include <sys/types.h>

#include "ConfigStructure.h"

typedef u_int8_t (*generator)();

void cipher(u_int8_t *plain_text, u_int8_t *const cipher_stream,
            u_int64_t length, u_int64_t cipher_stream_size,
            u_int64_t cipher_stream_index, u_int8_t *feedback);

void refill_cipher_stream(u_int8_t *const cipher_stream, u_int64_t amount,
                          u_int64_t cipher_stream_size,
                          u_int64_t cipher_stream_index, generator gen);
#endif  //__CIPHER_H