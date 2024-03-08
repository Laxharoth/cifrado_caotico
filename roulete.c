#include "roulete.h"

void initilizale_roulete(const Configuracion *const config,
                         struct rouleteConfig *roulete_config) {
    const size_t numeroMapas = config->num_maps;
    const uint64_t num_mapas_mask = numeroMapas - 1;
    const uint64_t table_mask = numeroMapas - 1;
    const uint64_t raw = (numeroMapas * config->n) - 1;
    // reservar memoria para las tablas
    roulete_config->raw = malloc(numeroMapas * config->n * sizeof(uint64_t));
    roulete_config->roulete =
        malloc(sizeof(struct chaotic_lookup_table) * numeroMapas);
    // inicializa atributos de la configuración de ruleta
    roulete_config->num_mapas_mask = num_mapas_mask;
    roulete_config->lu_table_mask = table_mask;
    roulete_config->lu_raw_mask = raw;
    roulete_config->epsilon = epsilon;
    roulete_config->roulette_selector = 0;
    roulete_config->lu_table_position = 0;
    roulete_config->H = 0;
    // LLenar las tablas
    srand(config->seed);
    uint64_t *Xn = (uint64_t *)roulete_config->raw;
    struct chaotic_lookup_table *roulete = roulete_config->roulete;
    uint64_t parametros[numeroMapas];
    uint64_t parametros_l[numeroMapas];
    for (size_t i = 0; i < numeroMapas; i++) {
        Xn[i * config->n] = rand();
        Xn[i * config->n] = (Xn[i] << 32) | rand();
        parametros[i] = (rand() % 3000) + 5;
        parametros_l[i] = (rand() & 63) + 5;
        for (size_t j = 1; j < config->n; ++j) {
            Xn[i * config->n + j] = RenyiMap(Xn[i * config->n + j - 1],
                                             parametros[i], parametros_l[i]);
        }
        roulete[i].lookup_table = Xn + i * config->n;
        roulete[i].parameter_r = parametros[i];
        roulete[i].parameter_l = parametros_l[i];
        roulete[i].last_generated = Xn[config->n - 1];
    }
}

void delete_roulete(struct rouleteConfig *config) {
    free(config->raw);
    free(config->roulete);
}

uint64_t random_select_coupled_chaotic_map_lookuptable(
    struct rouleteConfig *roulete_config) {
    uint64_t *ref_roulete_position = &roulete_config->roulette_selector;
    struct chaotic_lookup_table *roulete = roulete_config->roulete;
    struct chaotic_lookup_table *Yn = &roulete[*ref_roulete_position];
    uint64_t *table_position = &roulete_config->lu_table_position;
    const uint64_t lu_table_position =
        roulete_config->raw[(*table_position)] & roulete_config->lu_table_mask;
    (*table_position) = ((*table_position) + 1) & roulete_config->lu_raw_mask;
    Yn->last_generated =
        RenyiMap(Yn->last_generated, Yn->parameter_r, Yn->parameter_l) +
        (roulete_config->epsilon & (roulete_config->H));
    roulete_config->H ^= Yn->last_generated;
    *ref_roulete_position = Yn->last_generated & roulete_config->num_mapas_mask;
    const uint64_t ret_val = Yn->lookup_table[lu_table_position];
    Yn->lookup_table[lu_table_position] = Yn->last_generated;
    return ret_val;
}

void roulete_generator(uint64_t *const buffer, const Configuracion *config,
                       struct rouleteConfig *rouleteConfig) {
    const size_t chunk_size =
        sizeof(uint64_t);  // Tamaño de cada fragmento a escribir
    size_t remaining_bytes = config->file_size;
    uint8_t *ptr_buffer = (uint8_t *)buffer;

    while (remaining_bytes > 0) {
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;
        const uint64_t generated =
            random_select_coupled_chaotic_map_lookuptable(rouleteConfig);
        memcpy(ptr_buffer, &generated, chunk_bytes);
        ptr_buffer += chunk_bytes;
        remaining_bytes -= chunk_bytes;
    }
}