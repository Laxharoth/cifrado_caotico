#include "roulete.h"

void generate_seed(const Configuracion *const config, uint64_t *const seed) {
    srand(config->seed);
    const uint64_t numeroMapas = config->num_maps;
    uint64_t *parametros = seed;
    uint64_t *X0 = parametros + numeroMapas;
    uint64_t *parametros_l = X0 + numeroMapas;

    for (size_t i = 0; i < numeroMapas; i++) {
        X0[i] = rand();
        X0[i] = (X0[i] << 32) | rand();
        parametros[i] = rand();
        parametros[i] = (parametros[i] << 32) | rand();
        parametros_l[i] = (rand() + 5) & 63;
    }
}

void initilizale_roulete(const Configuracion *const config,
                         struct rouleteConfig *roulete_config,
                         const uint64_t *const seed) {
    const size_t numeroMapas = config->num_maps;
    // reservar memoria para las tablas
    roulete_config->raw =
        (uint8_t *)malloc(numeroMapas * config->n * sizeof(uint64_t));
    memset(roulete_config->raw, 0, numeroMapas * config->n * sizeof(uint64_t));
    roulete_config->roulete = (struct chaotic_lookup_table *)malloc(
        sizeof(struct chaotic_lookup_table) * numeroMapas);
    roulete_config->roulete_cycle = (struct chaotic_lookup_table **)malloc(
        sizeof(struct chaotic_lookup_table *) * 257);
    // inicializa atributos de la configuración de ruleta
    roulete_config->lu_table_mask = config->n - 1;
    roulete_config->lu_raw_mask = (numeroMapas * config->n) - 1;
    roulete_config->epsilon = epsilon;
    roulete_config->roulette_selector = 0;
    roulete_config->lu_table_position = 0;
    roulete_config->H = 0;
    // LLenar las tablas
    const uint64_t *parametros = seed;
    const uint64_t *X0 = parametros + numeroMapas;
    const uint64_t *parametros_l = X0 + numeroMapas;

    uint64_t *Xn = (uint64_t *)roulete_config->raw;
    struct chaotic_lookup_table *roulete = roulete_config->roulete;
    // Coloca las condiciones iniciales
    for (size_t i = 0; i < numeroMapas; i++) Xn[i * numeroMapas] = X0[i];
    // Calcula las primeras k (10) trayectorias acopladas
    for (size_t i = 0; i < 10; i++)
        for (size_t map_iter = 0; map_iter < numeroMapas; map_iter++) {
            Xn[map_iter * numeroMapas] =
                RenyiMap(Xn[map_iter * numeroMapas], parametros[map_iter],
                         parametros_l[map_iter]) +
                (roulete_config->epsilon & (roulete_config->H));
            roulete_config->H ^= Xn[map_iter * numeroMapas];
        }
    // Llena las n trayectorias de cada mapa
    for (size_t i = 0; i < numeroMapas; i++) {
        for (size_t j = 1; j < config->n; ++j) {
            Xn[i * config->n + j] =
                RenyiMap(Xn[i * config->n + j - 1], parametros[i],
                         parametros_l[i]) +
                (roulete_config->epsilon & (roulete_config->H));
            roulete_config->H ^= Xn[i * config->n + j];
        }
        roulete[i].lookup_table = Xn + i * config->n;
        roulete[i].parameter_r = parametros[i];
        roulete[i].parameter_l = parametros_l[i];
        roulete[i].last_generated = Xn[config->n - 1];
    }
    // Coloca los mapas en un arreglo de arreglos de tamaño fijo
    for (size_t i = 0; i < 257; ++i) {
        roulete_config->roulete_cycle[i] =
            roulete_config->roulete + (i % config->num_maps);
    }
}

void delete_roulete(struct rouleteConfig *config) {
    free(config->roulete_cycle);
    free(config->roulete);
    free(config->raw);
}

uint64_t random_select_coupled_chaotic_map_lookuptable(
    struct rouleteConfig *roulete_config) {
    uint8_t *ref_roulete_position = &roulete_config->roulette_selector;
    struct chaotic_lookup_table **roulete = roulete_config->roulete_cycle;
    struct chaotic_lookup_table *Yn = roulete[*ref_roulete_position];
    uint64_t *table_position = &roulete_config->lu_table_position;
    const uint64_t lu_table_position =
        roulete_config->raw[(*table_position)] & roulete_config->lu_table_mask;
    (*table_position) = ((*table_position) + 1) & roulete_config->lu_raw_mask;
    Yn->last_generated =
        RenyiMap(Yn->last_generated, Yn->parameter_r, Yn->parameter_l) +
        (roulete_config->epsilon & (roulete_config->H));
    roulete_config->H ^= Yn->last_generated;
    *ref_roulete_position = (uint8_t)Yn->last_generated;
    const uint64_t ret_val = Yn->lookup_table[lu_table_position];
    Yn->lookup_table[lu_table_position] = Yn->last_generated;
    return ret_val;
}

uint64_t random_select_coupled_chaotic_map_lookuptable_bitoffset(
    struct rouleteConfig *roulete_config) {
    // Obtiene mascara para el offset de combinación de numeros
    const uint8_t mask_offset = (sizeof(uint64_t) << 3) - 1;
    // copia pointers de la configuración a locales para mejor legibilidad
    uint8_t *ref_roulete_position = &roulete_config->roulette_selector;
    struct chaotic_lookup_table **roulete = roulete_config->roulete_cycle;
    uint64_t *table_position = &roulete_config->lu_table_position;
    // Obtiene lookup tables que se van a utilizar
    struct chaotic_lookup_table *Yn = *(roulete + *ref_roulete_position);
    struct chaotic_lookup_table *Yn_plus1 =
        *(roulete + *ref_roulete_position + 1);
    // Obtiene la posición del numero aleatorio
    const uint64_t lu_table_position =
        roulete_config->raw[(*table_position)] & roulete_config->lu_table_mask;
    (*table_position) = ((*table_position) + 1) & roulete_config->lu_raw_mask;
    // Obtiene el offset para combinar los numeros aleatorios
    const uint64_t offset =
        roulete_config->raw[(*table_position)] & mask_offset;
    (*table_position) = ((*table_position) + 1) & roulete_config->lu_raw_mask;
    // Crea mascara para reemplazar el los bits usados
    const uint64_t mask_replace = (~(0ull)) >> offset;

    // Coloca los numeros que se van a usar en punteros
    uint64_t *const num_1 = Yn->lookup_table + lu_table_position;
    uint64_t *const num_2 = Yn_plus1->lookup_table + lu_table_position;

    // Genera el proximo numero aleatorio
    Yn->last_generated =
        RenyiMap(Yn->last_generated, Yn->parameter_r, Yn->parameter_l) +
        (roulete_config->epsilon & (roulete_config->H));
    // Actualiza la perturbacion
    roulete_config->H ^= Yn->last_generated;
    // Actualiza el ultimo generado de la ruleta seleccionada
    *ref_roulete_position = (uint8_t)Yn->last_generated;
    // Calcula el numero aleatorio utilizando la operación de cruce
    const uint64_t ret_val =
        ((*num_1) & mask_replace) | ((*num_2) & ~mask_replace);
    // Reemplaza los bits aleatorios usados con los de la nueva trayectoria
    // generada
    *num_1 =
        ((*num_1) & (~mask_replace)) | (Yn->last_generated & (mask_replace));
    *num_2 =
        ((*num_2) & (mask_replace)) | (Yn->last_generated & (~mask_replace));
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
            // random_select_coupled_chaotic_map_lookuptable(rouleteConfig);
            random_select_coupled_chaotic_map_lookuptable_bitoffset(
                rouleteConfig);
        memcpy(ptr_buffer, &generated, chunk_bytes);
        ptr_buffer += chunk_bytes;
        remaining_bytes -= chunk_bytes;
    }
}