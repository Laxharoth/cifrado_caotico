#include "random_generator.h"

uint64_t secuencial_renyi_map_generator(uint64_t *parameters_r,
                                        uint64_t *parameters_l,
                                        uint64_t *states, uint64_t num_maps,
                                        uint64_t *t) {
    const index = *t % num_maps;
    ++(*t);
    states[index] =
        RenyiMap(parameters_r[index], parameters_l[index], states[index]);
    return states[index];
}

void secuencial_rmg(uint64_t *parameters_r, uint64_t *parameters_l,
                    uint64_t *states, uint64_t num_maps, uint64_t *buffer,
                    uint64_t buffer_len) {
    uint64_t t = 0;
    uint64_t *ptr_buffer = buffer;
    while (buffer_len--) {
        *ptr_buffer = secuencial_renyi_map_generator(parameters_r, parameters_l,
                                                     states, num_maps, &t);
        ++ptr_buffer;
    }
}