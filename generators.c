#include "generators.h"

void generate_random_file_1(unsigned char *const buffer,
                            const Configuracion *config) {
    // Obtener condiciones iniciales
    ull X[config->num_maps];
    for (size_t i = 0; i < config->num_maps * 2; ++i) {
        ((uint *)X)[i] = rand();
    }
    printf("JAJA Que loquillo me estan llamando");
    unsigned char *ptr_buffer = buffer;

    const size_t chunk_size =
        sizeof(ull) * config->num_maps;  // Tamaño de cada fragmento a escribir
    size_t remaining_bytes = config->file_size;
    while (remaining_bytes > 0) {
        // Generar datos aleatorios para el fragmento actual
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;
        renyi_array_generator(X, config->beta, config->lambda,
                              config->num_maps);
        memcpy(ptr_buffer, X, chunk_bytes);
        ptr_buffer += chunk_bytes;

        remaining_bytes -= chunk_bytes;
    }
}

void generate_random_file_2(unsigned char *const buffer,
                            const Configuracion *config) {
    // Obtener condiciones iniciales
    ull X[config->num_maps];
    for (size_t i = 0; i < config->num_maps * 2; ++i) {
        ((uint *)X)[i] = rand();
    }
    ull Yi;
    unsigned char *ptr_buffer = buffer;

    const size_t chunk_size = sizeof(ull);
    size_t remaining_bytes = config->file_size;
    while (remaining_bytes > 0) {
        // Generar datos aleatorios para el fragmento actual
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;

        Yi = renyi_array_combine_maps_with_mask_and_replace(
            X, config->beta, config->lambda, config->num_maps - 1);
        memcpy(ptr_buffer, &Yi, chunk_bytes);
        ptr_buffer += chunk_bytes;
        remaining_bytes -= chunk_bytes;
    }
}

void generate_random_file_3(unsigned char *const buffer,
                            const Configuracion *config) {
    // Obtener condiciones iniciales
    ull X[config->num_maps];
    for (size_t i = 0; i < config->num_maps * 2; ++i) {
        ((uint *)X)[i] = rand();
    }
    ull Yi;

    unsigned char *ptr_buffer = buffer;

    const size_t chunk_size = sizeof(ull);
    size_t remaining_bytes = config->file_size;
    while (remaining_bytes > 0) {
        // Generar datos aleatorios para el fragmento actual
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;

        Yi = renyi_array_random_byte_select_with_replace(
            X, config->beta, config->lambda, (config->num_maps * 8) - 1);
        memcpy(ptr_buffer, &Yi, chunk_bytes);
        ptr_buffer += chunk_bytes;

        remaining_bytes -= chunk_bytes;
    }
}

void generate_random_file_4(unsigned char *const buffer,
                            const Configuracion *config) {
    srand(config->seed);
    ull X = rand();
    X = (X << 32) | rand();

    unsigned char *ptr_buffer = buffer;

    const size_t chunk_size =
        sizeof(ull);  // Tamaño de cada fragmento a escribir
    size_t remaining_bytes = config->file_size;
    ull divisor = config->r;
    ull t = (MAX_ULL / divisor);
    t = sqrtull(t);
    while (remaining_bytes > 0) {
        // Generar datos aleatorios para el fragmento actual
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;
        X = logistic_renyi(X, config->beta, config->lambda, config->r, t);
        memcpy(ptr_buffer, &X, chunk_bytes);
        ptr_buffer += chunk_bytes;

        remaining_bytes -= chunk_bytes;
    }
}

void generate_random_file_5(unsigned char *const buffer,
                            const Configuracion *config) {
    srand(config->seed);

    ull X = rand();
    X = (X << 32) | rand();
    ull Yi[255];
    unsigned char *ptr_buffer = buffer;

    ull size = config->n;

    const size_t chunk_size =
        sizeof(ull) * size;  // Tamaño de cada fragmento a escribir
    size_t remaining_bytes = config->file_size;
    ull t = (MAX_ULL / config->r);
    t = sqrtull(t);
    while (remaining_bytes > 0) {
        // Generar datos aleatorios para el fragmento actual
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;
        X = logistic_renyi_with_cycle(X, Yi, config->beta, config->lambda,
                                      config->r, t, size);

        memcpy(ptr_buffer, Yi, chunk_bytes);
        ptr_buffer += chunk_bytes;

        remaining_bytes -= chunk_bytes;
    }
}

void generate_random_file_6(unsigned char *const buffer,
                            const Configuracion *config) {
    srand(config->seed);

    ull X = rand();
    X = (X << 32) | rand();
    ull Yi[255];
    unsigned char *ptr_buffer = buffer;
    ull mask = 0b1111;

    ull divisor = config->r;
    size_t remaining_bytes = config->file_size;
    ull t = (MAX_ULL / divisor);
    t = sqrtull(t);
    while (remaining_bytes > 0) {
        const ull size = (X & mask) | 1;
        const size_t chunk_size =
            sizeof(ull) * size;  // Tamaño de cada fragmento a escribir
        // Generar datos aleatorios para el fragmento actual
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;
        X = logistic_renyi_with_random_cycle(
            X, Yi, config->beta, config->lambda, config->r, t, mask);

        memcpy(ptr_buffer, Yi, chunk_bytes);
        ptr_buffer += chunk_bytes;

        remaining_bytes -= chunk_bytes;
    }
}

void generate_random_file_7(unsigned char *const buffer,
                            const Configuracion *config) {
    srand(config->seed);

    ull X = rand();
    X = (X << 32) | rand();

    const size_t chunk_size =
        sizeof(ull);  // Tamaño de cada fragmento a escribir
    size_t remaining_bytes = config->file_size;
    ull divisor = config->r;
    ull t = (MAX_ULL / divisor);
    t = sqrtull(t);

    unsigned char *ptr_buffer = buffer;

    while (remaining_bytes > 0) {
        // Generar datos aleatorios para el fragmento actual
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;
        X = logistic_generalized(X, config->h, config->k, config->n);

        memcpy(ptr_buffer, &X, chunk_bytes);
        ptr_buffer += chunk_bytes;

        remaining_bytes -= chunk_bytes;
    }
}

void generate_random_file_8(unsigned char *const buffer,
                            const Configuracion *config) {
    const size_t numeroMapas = 4;

    ull Xn[numeroMapas], parametros[numeroMapas], epsilon = 65535, j = 5, H = 0;

    srand(config->seed);
    for (size_t i = 0; i < numeroMapas; i++) {
        Xn[i] = rand();
        Xn[i] = (Xn[i] << 32) | rand();
        parametros[i] = (rand() % 3000) + 5;
    }

    unsigned char *ptr_buffer = buffer;

    const size_t chunk_size =
        sizeof(ull) * numeroMapas;  // Tamaño de cada fragmento a escribir
    size_t remaining_bytes = config->file_size;
    while (remaining_bytes > 0) {
        // Generar datos aleatorios para el fragmento actual
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;
        SecureReal_TimeChaoticPartialEncryptionGenerator(
            Xn, parametros, j, epsilon, &H, numeroMapas);

        memcpy(ptr_buffer, Xn, chunk_bytes);
        ptr_buffer += chunk_bytes;

        remaining_bytes -= chunk_bytes;
    }
}

void generate_random_file_9(unsigned char *const buffer,
                            const Configuracion *config) {
    srand(config->seed);
    ull X = rand();
    X = (X << 32) | rand();

    const size_t chunk_size =
        sizeof(ull);  // Tamaño de cada fragmento a escribir
    size_t remaining_bytes = config->file_size;
    ull index = 0;
    while (remaining_bytes > 0) {
        // Generar datos aleatorios para el fragmento actual
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;
        buffer[index++] = X = RenyiMap(X, config->beta, config->lambda);

        remaining_bytes -= chunk_bytes;
    }
}

void generate_random_file_10(unsigned char *const buffer,
                             const Configuracion *config) {
    srand(config->seed);
    ull X = rand();
    X = (X << 32) | rand();

    const size_t chunk_size =
        sizeof(ull);  // Tamaño de cada fragmento a escribir
    size_t remaining_bytes = config->file_size;
    ull t = (MAX_ULL / config->r);
    t = sqrtull(t);
    ull index = 0;
    while (remaining_bytes > 0) {
        // Generar datos aleatorios para el fragmento actual
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;
        buffer[index++] = X = LogisticMapInt(X, config->r, t);

        remaining_bytes -= chunk_bytes;
    }
}

void generate_random_file_11(unsigned char *const buffer,
                             const Configuracion *config) {
    const size_t numeroMapas = 4;
    ull Xn[numeroMapas], parametros[numeroMapas], j = 5;

    srand(config->seed);
    for (size_t i = 0; i < numeroMapas; i++) {
        Xn[i] = rand();
        Xn[i] = (Xn[i] << 32) | rand();
        parametros[i] = (rand() % 3000) + 5;
    }

    const size_t chunk_size =
        sizeof(ull);  // Tamaño de cada fragmento a escribir
    size_t remaining_bytes = config->file_size;
    ull position = 0;
    ull index = 0;
    while (remaining_bytes > 0) {
        // Generar datos aleatorios para el fragmento actual
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;
        buffer[index++] = random_select_coupled_chaotic_map(
            &position, Xn, parametros, j, 0b11);

        remaining_bytes -= chunk_bytes;
    }
}

void generate_random_file_12(unsigned char *const buffer,
                             const Configuracion *config) {
    const size_t numeroMapas = 4;
    ull Xn[numeroMapas], parametros[numeroMapas], epsilon = 65535, j = 5, H = 0;

    srand(config->seed);
    for (size_t i = 0; i < numeroMapas; i++) {
        Xn[i] = rand();
        Xn[i] = (Xn[i] << 32) | rand();
        parametros[i] = (rand() % 3000) + 5;
    }

    const size_t chunk_size =
        sizeof(ull);  // Tamaño de cada fragmento a escribir
    size_t remaining_bytes = config->file_size;
    ull position = 0;
    ull index = 0;
    while (remaining_bytes > 0) {
        // Generar datos aleatorios para el fragmento actual
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;
        buffer[index++] = random_select_coupled_chaotic_map_with_perturbation(
            &position, Xn, parametros, j, epsilon, &H, 0b11);

        remaining_bytes -= chunk_bytes;
    }
}
void generate_random_file_13(unsigned char *const buffer,
                             const Configuracion *config) {
    const size_t numeroMapas = config->num_maps;
    const ull num_mapas_mask = numeroMapas - 1;
    const ull table_size = config->n;
    const ull table_mask = table_size - 1;
    const ull total_table_size = numeroMapas * table_size;
    unsigned char *ptr_buffer = buffer;
    ull *Xn = malloc(sizeof(ull) * total_table_size),
        *parametros = malloc(sizeof(ull) * numeroMapas), epsilon = 65535,
        lambda = 5, H = 0;
    chaotic_lookup_table roulete[numeroMapas];
    const ull chaotic_table_size_mask = numeroMapas * config->n - 1;
    srand(config->seed);

    for (size_t i = 0; i < numeroMapas; i++) {
        Xn[i * config->n] = rand();
        Xn[i * config->n] = (Xn[i * config->n] << 32) | rand();
        parametros[i] = (rand() % 3000) + 5;
        for (size_t j = 1; j < config->n; ++j) {
            Xn[i * config->n + j] =
                RenyiMap(Xn[i * config->n + j - 1], parametros[i], lambda);
        }
        roulete[i].lookup_table = Xn + i * config->n;
        roulete[i].lu_table_size = config->n;
        roulete[i].lu_table_mask = table_mask;
        roulete[i].last_generated = Xn[i * config->n + config->n - 1];
    }

    const size_t chunk_size =
        sizeof(ull);  // Tamaño de cada fragmento a escribir
    size_t remaining_bytes = config->file_size;
    static ull position = 0;
    static ull i_lut_pos_list_index = 0;

    while (remaining_bytes > 0) {
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;
        const ull generated = random_select_coupled_chaotic_map_lookuptable(
            &position,
            (Xn[(i_lut_pos_list_index++) & chaotic_table_size_mask]) &
                table_mask,
            roulete, parametros, lambda, num_mapas_mask, epsilon, &H);
        memcpy(ptr_buffer, &generated, chunk_bytes);
        ptr_buffer += chunk_bytes;
        remaining_bytes -= chunk_bytes;
    }
    free(Xn);
    free(parametros);
}
void generate_random_file_14(unsigned char *const buffer,
                             const Configuracion *config) {
    const size_t numeroMapas = config->num_maps;
    const ull num_mapas_mask = numeroMapas - 1;
    const ull table_mask = (config->n << 3) - 1;
    unsigned char *ptr_buffer = buffer;
    const ull table_size = config->n;
    const ull total_table_size = numeroMapas * table_size;
    ull *Xn = malloc(sizeof(ull) * total_table_size),
        *parametros = malloc(sizeof(ull) * numeroMapas), epsilon = 65535,
        lambda = 5, H = 0;
    chaotic_lookup_table roulete[numeroMapas];
    srand(config->seed);
    for (size_t i = 0; i < numeroMapas; i++) {
        Xn[i * (config->n + 1)] = rand();
        Xn[i * (config->n + 1)] = (Xn[i] << 32) | rand();
        parametros[i] = (rand() % 3000) + 5;
        for (size_t j = 1; j < (config->n + 1); ++j) {
            Xn[i * (config->n + 1) + j] =
                RenyiMap(Xn[i * (config->n + 1) + j - 1], parametros[i], j);
        }
        roulete[i].lookup_table = Xn + i * (config->n + 1);
        roulete[i].lu_table_size = config->n;
        roulete[i].lu_table_mask = table_mask;
        roulete[i].last_generated = Xn[i * (config->n + 1) + config->n - 1];
    }

    const size_t chunk_size =
        sizeof(ull);  // Tamaño de cada fragmento a escribir
    size_t remaining_bytes = config->file_size;
    ull position = 0;
    ull index = 0;
    ull lu_table_position_list[config->n];
    const ull increase_shift_lut_pos_list = countBitsSet(table_mask);
    ull current_shift_lut_pos_list;
    ull i_lut_pos_list_index;
    ull select_map_for_lut_pos_list = 0;
    ull select_indx_for_lut_pos_list = 0;
    while (remaining_bytes > 0) {
        // Crear lista de numeros aleatorios para seleccionar el
        // indice del mapa
        const size_t table_1 = roulete[select_map_for_lut_pos_list]
                                   .lookup_table[select_indx_for_lut_pos_list] &
                               num_mapas_mask;
        select_map_for_lut_pos_list =
            (select_map_for_lut_pos_list + 1) & num_mapas_mask;
        select_indx_for_lut_pos_list =
            (select_indx_for_lut_pos_list + 1) & table_mask;
        const size_t table_2 = roulete[select_map_for_lut_pos_list]
                                   .lookup_table[select_indx_for_lut_pos_list] &
                               num_mapas_mask;
        select_map_for_lut_pos_list =
            (select_map_for_lut_pos_list + 1) & num_mapas_mask;
        select_indx_for_lut_pos_list =
            (select_indx_for_lut_pos_list + 1) & table_mask;
        // Generar datos aleatorios para el fragmento actual
        for (ull i = 0; i < config->n; ++i) {
            lu_table_position_list[i] = roulete[table_1].lookup_table[i] ^
                                        roulete[table_2].lookup_table[i];
        }
        // --
        for (i_lut_pos_list_index = 0;
             i_lut_pos_list_index < config->n && remaining_bytes;
             ++i_lut_pos_list_index) {
            current_shift_lut_pos_list = 0;
            for (ull current_shift_lut_pos_list = 0;
                 current_shift_lut_pos_list < (sizeof(ull) << 3) &&
                 remaining_bytes;
                 current_shift_lut_pos_list += increase_shift_lut_pos_list) {
                size_t chunk_bytes = (chunk_size < remaining_bytes)
                                         ? chunk_size
                                         : remaining_bytes;
                const ull generated =
                    random_select_coupled_chaotic_map_lookuptable_byte(
                        &position,
                        (lu_table_position_list[i_lut_pos_list_index] >>
                         current_shift_lut_pos_list) &
                            table_mask,
                        roulete, parametros, lambda, num_mapas_mask, epsilon,
                        &H);
                memcpy(ptr_buffer, &generated, chunk_bytes);
                ptr_buffer += chunk_bytes;
                remaining_bytes -= chunk_bytes;
            }
        }
    }
    free(Xn);
    free(parametros);
}

struct roulete_config {
    uint64_t **roulete;
    uint64_t *last_generated;
    uint64_t num_mapas_mask;
};

void generate_random_file_15(unsigned char *const buffer,
                             const Configuracion *config) {
    const size_t numeroMapas = config->num_maps;
    const ull num_mapas_mask = numeroMapas - 1;
    unsigned char *ptr_buffer = buffer;
    const ull table_size = config->n;
    const ull total_table_size = numeroMapas * table_size;
    ull epsilon = 65535, lambda = 5, H = 0;
    ull *Xn = malloc(sizeof(ull) * total_table_size);
    ull *parametros = malloc(sizeof(ull) * numeroMapas);
    chaotic_lookup_table roulete[numeroMapas];
    srand(config->seed);
    const ull table_mask = config->n - 1;
    for (size_t i = 0; i < numeroMapas; i++) {
        Xn[i * config->n] = rand();
        Xn[i * config->n] = (Xn[i] << 32) | rand();
        parametros[i] = (rand() % 3000) + 5;
        for (size_t j = 1; j < config->n; ++j) {
            Xn[i * config->n + j] =
                RenyiMap(Xn[i * config->n + j - 1], parametros[i], lambda);
        }
        roulete[i].lookup_table = Xn + i * (config->n + 1);
        roulete[i].lu_table_size = config->n;
        roulete[i].lu_table_mask = table_mask;
        roulete[i].last_generated = Xn[config->n - 1];
    }

    size_t remaining_bytes = config->file_size;
    ull roulete_position = 0;
    ull table_position = 0;
    ull index = 0;

    ull select_map_for_lut_pos_list = 0;
    ull select_indx_for_lut_pos_list = 0;

    const ull mask_lut_pos_indxsize =
        (config->n * numeroMapas * sizeof(ull)) - 1;
    const ull chunk_size = sizeof(ull);
    unsigned u = 0;
    while (remaining_bytes > 0) {
        ull generated =
            random_select_coupled_chaotic_map_lookuptable_horizontal_perturbation(
                roulete, &roulete_position, num_mapas_mask, table_mask,
                &table_position, Xn, mask_lut_pos_indxsize, parametros, lambda,
                epsilon, &H);
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;
        memcpy(ptr_buffer, &generated, chunk_bytes);
        ptr_buffer += chunk_bytes;
        remaining_bytes -= chunk_bytes;
    }

    free(Xn);
}