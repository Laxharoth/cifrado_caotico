#ifndef __ROULETE_H__
#define __ROULETE_H__

#include <stdint.h>
#include <stdlib.h>

#include "ConfigStructure.h"
#include "chaotic_map.h"

const static uint64_t epsilon = 65535;
struct chaotic_lookup_table {
    uint64_t *lookup_table;
    uint64_t parameter_r;
    uint64_t parameter_l;
    uint64_t last_generated;
};

struct rouleteConfig {
    struct chaotic_lookup_table *roulete;
    struct chaotic_lookup_table **roulete_cycle;
    uint8_t *raw;
    uint64_t lu_table_mask;
    uint64_t lu_raw_mask;
    uint64_t epsilon;
    uint8_t roulette_selector;
    uint64_t lu_table_position;
    uint64_t H;
};
void generate_seed(const Configuracion *const config, uint64_t *const seed);
void initilizale_roulete(const Configuracion *const config,
                         struct rouleteConfig *roulete_config,
                         const uint64_t *const seed);
void delete_roulete(struct rouleteConfig *config);
uint64_t random_select_coupled_chaotic_map_lookuptable(
    struct rouleteConfig *roulete_config);
void roulete_generator(uint64_t *const buffer, const Configuracion *config,
                       struct rouleteConfig *rouleteConfig);

#endif  //__ROULETE_H__