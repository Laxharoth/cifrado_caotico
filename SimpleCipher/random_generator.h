#ifndef RANDOM_GENERATOR_H
#define RANDOM_GENERATOR_H
#include <stdint.h>

#include "../chaotic_map.h"

uint64_t secuencial_renyi_map_generator(uint64_t *parameters_r,
                                        uint64_t *parameters_l,
                                        uint64_t *states, uint64_t num_maps,
                                        uint64_t *t);
void secuencial_rmg(uint64_t *parameters_r, uint64_t *parameters_l,
                    uint64_t *states, uint64_t num_maps, uint64_t *buffer,
                    uint64_t buffer_len);
#endif /* RANDOM_GENERATOR_H */
