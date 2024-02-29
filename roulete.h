#ifndef __ROULETE_H__
#define __ROULETE_H__

#include <stdint.h>
#include <stdlib.h>

#include "ConfigStructure.h"
const static uint64_t epsilon = 65535;
struct chaotic_lookup_table {
    uint64_t *lookup_table;
    uint64_t parameter_r;
    uint64_t parameter_l;
    uint64_t last_generated;
};

struct rouleteConfig {
    struct chaotic_lookup_table *roulete;
    uint8_t *raw;
    uint64_t num_mapas_mask;
    uint64_t lu_table_mask;
    uint64_t lu_raw_mask;
    uint64_t epsilon;
    uint64_t roulette_selector;
    uint64_t lu_table_position;
    uint64_t H;
};

#define RenyiMap(X, beta, renyi_lambda) (X * beta) + (X >> renyi_lambda)

void initilizale_roulete(const Configuracion *const config,
                         struct rouleteConfig *roulete_config);
uint64_t random_select_coupled_chaotic_map_lookuptable(
    struct rouleteConfig *roulete_config);
void roulete_generator(unsigned char *const buffer, const Configuracion *config,
                       struct rouleteConfig *rouleteConfig);

#endif  //__ROULETE_H__