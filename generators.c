#include "ConfigStructure.h"
#include "fn.c"

void generate_random_file_1(unsigned char *const buffer,
                            const Configuracion *config) {
    typedef unsigned long long ull;

    // Obtener condiciones iniciales
    ull X[NUMBER_OF_CAHOTIC_MAPS];
    for (size_t i = 0; i < NUMBER_OF_CAHOTIC_MAPS * 2; ++i) {
        ((ull *)X)[i] = rand();
    }

    unsigned char *ptr_buffer = buffer;

    const size_t chunk_size =
        sizeof(ull) *
        NUMBER_OF_CAHOTIC_MAPS;  // Tamaño de cada fragmento a escribir
    size_t remaining_bytes = config->file_size;
    while (remaining_bytes > 0) {
        // Generar datos aleatorios para el fragmento actual
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;
        renyi_array_generator(X, config->beta, config->lambda);
        memcpy(ptr_buffer, X, chunk_bytes);
        ptr_buffer += chunk_bytes;

        remaining_bytes -= chunk_bytes;
    }
}

void generate_random_file_2(unsigned char *const buffer,
                            const Configuracion *config) {
    typedef unsigned long long ull;

    // Obtener condiciones iniciales
    ull X[NUMBER_OF_CAHOTIC_MAPS];
    for (size_t i = 0; i < NUMBER_OF_CAHOTIC_MAPS * 2; ++i) {
        ((ull *)X)[i] = rand();
    }
    ull Yi;
    unsigned char *ptr_buffer = buffer;

    const size_t chunk_size = sizeof(ull);
    size_t remaining_bytes = config->file_size;
    while (remaining_bytes > 0) {
        // Generar datos aleatorios para el fragmento actual
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;

        Yi = renyi_array_combine_maps_with_mask_and_replace(X, config->beta,
                                                            config->lambda);
        memcpy(ptr_buffer, &Yi, chunk_bytes);
        ptr_buffer += chunk_bytes;
        remaining_bytes -= chunk_bytes;
    }
}

void generate_random_file_3(unsigned char *const buffer,
                            const Configuracion *config) {
    typedef unsigned long long ull;

    // Obtener condiciones iniciales
    ull X[NUMBER_OF_CAHOTIC_MAPS];
    for (size_t i = 0; i < NUMBER_OF_CAHOTIC_MAPS * 2; ++i) {
        ((ull *)X)[i] = rand();
    }
    ull Yi;

    unsigned char *ptr_buffer = buffer;

    const size_t chunk_size = sizeof(ull);
    size_t remaining_bytes = config->file_size;
    while (remaining_bytes > 0) {
        // Generar datos aleatorios para el fragmento actual
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;

        Yi = renyi_array_random_byte_select_with_replace(X, config->beta,
                                                         config->lambda);
        memcpy(ptr_buffer, &Yi, chunk_bytes);
        ptr_buffer += chunk_bytes;

        remaining_bytes -= chunk_bytes;
    }
}

void generate_random_file_4(unsigned char *const buffer,
                            const Configuracion *config) {
    typedef unsigned long long ull;

    ull X = rand();
    X = (X << 32) | rand();

    unsigned char *ptr_buffer = buffer;

    const size_t chunk_size =
        sizeof(ull);  // Tamaño de cada fragmento a escribir
    size_t remaining_bytes = config->file_size;
    ull divisor = config->r;
    ull t = ((~(0ull)) / divisor);
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
    typedef unsigned long long ull;

    ull X = rand();
    X = (X << 32) | rand();
    ull Yi[255];
    unsigned char *ptr_buffer = buffer;

    ull size = config->n;

    const size_t chunk_size =
        sizeof(ull) * size;  // Tamaño de cada fragmento a escribir
    size_t remaining_bytes = config->file_size;
    ull t = ((~(0ull)) / config->r);
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
    typedef unsigned long long ull;

    ull X = rand();
    X = (X << 32) | rand();
    ull Yi[255];
    unsigned char *ptr_buffer = buffer;
    ull mask = 0b1111;

    ull divisor = config->r;
    size_t remaining_bytes = config->file_size;
    ull t = ((~(0ull)) / divisor);
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
    typedef unsigned long long ull;

    ull X = rand();
    X = (X << 32) | rand();

    const size_t chunk_size =
        sizeof(ull);  // Tamaño de cada fragmento a escribir
    size_t remaining_bytes = config->file_size;
    ull divisor = config->r;
    ull t = ((~(0ull)) / divisor);
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
    typedef unsigned long long ull;

    const size_t numeroMapas = 4;

    ull Xn[numeroMapas], parametros[numeroMapas], epsilon = 65535, j = 5, H = 0;

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
    typedef unsigned long long ull;

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
    typedef unsigned long long ull;

    ull X = rand();
    X = (X << 32) | rand();

    const size_t chunk_size =
        sizeof(ull);  // Tamaño de cada fragmento a escribir
    size_t remaining_bytes = config->file_size;
    ull t = ((~(0ull)) / config->r);
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
    typedef unsigned long long ull;

    const size_t numeroMapas = 4;
    ull Xn[numeroMapas], parametros[numeroMapas], j = 5;

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
    typedef unsigned long long ull;

    const size_t numeroMapas = 4;
    ull Xn[numeroMapas], parametros[numeroMapas], epsilon = 65535, j = 5, H = 0;

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
    typedef unsigned long long ull;

    const size_t numeroMapas = 4;
    const ull num_mapas_mask = 0b11;
    unsigned char *ptr_buffer = buffer;
    ull Xn[numeroMapas * config->n], parametros[numeroMapas], epsilon = 65535,
                                                              lambda = 5, H = 0;
    chaotic_lookup_table roulete[numeroMapas];
    const ull table_mask = config->n - 1;
    for (size_t i = 0; i < numeroMapas; i++) {
        Xn[i * config->n] = rand();
        Xn[i * config->n] = (Xn[i] << 32) | rand();
        parametros[i] = (rand() % 3000) + 5;
        for (size_t j = 1; j < config->n; ++j) {
            Xn[i * config->n + j] =
                RenyiMap(Xn[i * config->n + j - 1], parametros[i], lambda);
        }
        roulete[i].lookup_table = Xn;
        roulete[i].lu_table_size = config->n;
        roulete[i].lu_table_mask = table_mask;
        roulete[i].last_generated = Xn[config->n - 1];
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
        // Crear lista de numeros aleatorios para seleccionar el indice
        // del mapa
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
        for (ull i = 0; i < config->n; ++i) {
            lu_table_position_list[i] = roulete[table_1].lookup_table[i] ^
                                        roulete[table_2].lookup_table[i];
        }

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
                    random_select_coupled_chaotic_map_lookuptable(
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
}
void generate_random_file_14(unsigned char *const buffer,
                             const Configuracion *config) {
    typedef unsigned long long ull;

    const size_t numeroMapas = 4;
    const ull num_mapas_mask = 0b11;
    const ull table_mask = (config->n << 3) - 1;
    unsigned char *ptr_buffer = buffer;
    ull Xn[numeroMapas * (config->n + 1)], parametros[numeroMapas],
        epsilon = 65535, j = 5, H = 0;
    chaotic_lookup_table roulete[numeroMapas];
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
        // Crear lista de numeros aleatorios para seleccionar el indice
        // del mapa
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
                        roulete, parametros, j, num_mapas_mask, epsilon, &H);
                memcpy(ptr_buffer, &generated, chunk_bytes);
                ptr_buffer += chunk_bytes;
                remaining_bytes -= chunk_bytes;
            }
        }
    }
}

void generate_random_file_15(unsigned char *const buffer,
                             const Configuracion *config) {
    typedef unsigned long long ull;

    const size_t numeroMapas = 4;
    const ull num_mapas_mask = 0b11;
    unsigned char *ptr_buffer = buffer;
    ull Xn[numeroMapas * config->n], parametros[numeroMapas], epsilon = 65535,
                                                              lambda = 5, H = 0;
    chaotic_lookup_table roulete[numeroMapas];
    const ull table_mask = config->n - 1;
    for (size_t i = 0; i < numeroMapas; i++) {
        Xn[i * config->n] = rand();
        Xn[i * config->n] = (Xn[i] << 32) | rand();
        parametros[i] = (rand() % 3000) + 5;
        for (size_t j = 1; j < config->n; ++j) {
            Xn[i * config->n + j] =
                RenyiMap(Xn[i * config->n + j - 1], parametros[i], lambda);
        }
        roulete[i].lookup_table = Xn;
        roulete[i].lu_table_size = config->n;
        roulete[i].lu_table_mask = table_mask;
        roulete[i].last_generated = Xn[config->n - 1];
    }

    size_t remaining_bytes = config->file_size;
    ull position = 0;
    ull index = 0;

    const ull increase_shift_lut_pos_list = countBitsSet(table_mask);
    ull current_shift_lut_pos_list;
    ull i_lut_pos_list_index;
    ull select_map_for_lut_pos_list = 0;
    ull select_indx_for_lut_pos_list = 0;
    ull generated[410];
    const ull mask_lut_pos_indxsize =
        (table_mask << countBitsSet(num_mapas_mask)) | num_mapas_mask;
    ull number_of_generated_numbers = 0;
    while (remaining_bytes > 0) {
        random_select_coupled_chaotic_map_lookuptable_horizontal_perturbation(
            &position, roulete, num_mapas_mask, table_mask, Xn,
            mask_lut_pos_indxsize, parametros, lambda, epsilon, &H, generated,
            405, &number_of_generated_numbers,
            Xn[number_of_generated_numbers & mask_lut_pos_indxsize] & 0xFF);
        const ull chunk_size = sizeof(ull) * number_of_generated_numbers;
        size_t chunk_bytes =
            (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;
        memcpy(ptr_buffer, generated, chunk_bytes);
        ptr_buffer += chunk_bytes;
        remaining_bytes -= chunk_bytes;
    }
}
