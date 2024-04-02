#include "fn.h"

ull sqrtull(ull a) {
    ull min = 0;
    ull max = (1ull) << 32;

    while (1) {
        if (max <= 1 + min) return min;

        ull sqt = min + (max - min) / 2;
        ull sq = sqt * sqt;

        if (sq == a) return sqt;

        if (sq > a)
            max = sqt;
        else
            min = sqt;
    }
}

ull countBitsSet(ull num) {
    int count = 0;
    while (num) {
        count += num & 1;
        num >>= 1;
    }
    return count;
}

/**Rotacion izquierda circular*/
ull rotl64(ull value, unsigned int count) {
    const unsigned int mask = CHAR_BIT * sizeof(value) - 1;
    count &= mask;
    return (value << count) | (value >> (-count & mask));
}
/**Rotacion derecha circular*/
ull rotr64(ull value, unsigned int count) {
    const unsigned int mask = CHAR_BIT * sizeof(value) - 1;
    count &= mask;
    return (value >> count) | (value << (-count & mask));
}

void renyi_array_generator(ull *X, const unsigned int beta,
                           const unsigned int renyi_lambda, ull num_maps) {
    ull rn1, rn2, avr;
    rn1 = rn2 = 0;
    unsigned i = 0;
    ull *Xi;
    const int half = num_maps / 2;

    for (Xi = X, i = 0; i < half; i++, Xi++) {
        *Xi = RenyiMap(*Xi, beta, renyi_lambda);
        rn1 ^= *Xi;
    }

    byte *rotation = (byte *)(void *)&rn1;
    byte *bit_toggle = rotation + 4;

    for (Xi = X + half, i = half; i < num_maps;
         i++, Xi++, rotation++, bit_toggle++) {
        *Xi = RenyiMap(*Xi, beta, renyi_lambda);
        *Xi = rotl64(*Xi, (*rotation) & 63);
        *Xi ^= 1 << ((*bit_toggle) & 63);
        rn2 ^= *Xi;
    }
    avr = (rn1 ^ rn2);
    for (Xi = X, i = 0; i < num_maps; i++, Xi++) {
        *Xi = (*Xi) + (avr & 0b11111111);
    }
}

ull renyi_array_combine_maps_with_mask_and_replace(
    ull *X, const unsigned int beta, const unsigned int renyi_lambda,
    ull size_mask) {
    const ull mask = 0x00000000FFFFFFFFULL;
    static byte index_selector = 0;

    const byte index_1 = index_selector,
               index_2 = (index_selector + 1) & size_mask;
    const ull Y = (X[index_1] & mask) | (X[index_2] & ~mask);
    X[index_1] = RenyiMap(Y, beta, renyi_lambda);

    index_selector = index_2;
    return Y;
}

ull renyi_array_random_byte_select_with_replace(ull *X, const byte beta,
                                                const byte renyi_lambda,
                                                ull size_mask) {
    static byte index_selector = 0;
    ull Y;
    byte *ptr_YasBytes = (byte *)(void *)&Y;
    byte *ptr_XasBytes = (byte *)(void *)X;
    Y = *((ull *)(ptr_XasBytes + index_selector));
    index_selector = (*ptr_YasBytes & size_mask);
    index_selector = min(index_selector, 56);
    *((ull *)(ptr_XasBytes + index_selector)) = RenyiMap(Y, beta, renyi_lambda);
    return Y;
}

ull logistic_renyi(const ull x, const ull beta, const ull renyi_lambda,
                   const ull r, const ull t) {
    const ull renyi = RenyiMap(x, beta, renyi_lambda);
    return LogisticMapInt(renyi, r, t);
}

ull logistic_renyi_with_cycle(const ull x, ull *const _y, const ull beta,
                              const ull renyi_lambda, const ull r, const ull t,
                              const ull size) {
    ull *y = _y;
    *y = RenyiMap(x, beta, renyi_lambda);
    for (ull i = 1; i < size; ++i) {
        *(++y) = LogisticMapInt(*y, r, t) + 0xFFFF;
    }
    return *_y;
}

ull logistic_renyi_with_random_cycle(const ull x, ull *y, const ull beta,
                                     const ull renyi_lambda, const ull r,
                                     const ull t, const ull mask) {
    const ull size = (x & mask) | 1;
    *y = RenyiMap(x, beta, renyi_lambda);
    for (ull i = 1; i < size; ++i) {
        *(++y) = LogisticMapInt(*y, r, t);
    }
    return *y;
}

ull logistic_generalized(ull x, ull h, ull k, ull factor_n) {
    const ull difference = x - h;
    return k - difference * difference * factor_n;
}

void SecureReal_TimeChaoticPartialEncryptionGenerator(ull *Yn, ull *parametros,
                                                      ull j, ull epsilon,
                                                      ull *H, ull numMapas) {
    ull newH = 0;
    for (size_t i = 0; i < numMapas; i++) {
        Yn[i] = RenyiMap(Yn[i], parametros[i], j) + (epsilon & (*H));
        newH ^= Yn[i];
    }
    *H = newH;
}

ull random_select_coupled_chaotic_map(ull *ref_position, ull *Yn,
                                      ull *parametros, ull j,
                                      ull mask_numMapas) {
    ull ret_val = Yn[*ref_position] =
        RenyiMap(Yn[*ref_position], parametros[*ref_position], j);
    *ref_position = Yn[*ref_position] & mask_numMapas;
    return ret_val;
}

ull random_select_coupled_chaotic_map_with_perturbation(ull *ref_position,
                                                        ull *Yn,
                                                        ull *parametros, ull j,
                                                        ull epsilon, ull *ref_H,
                                                        ull mask_numMapas) {
    ull ret_val = Yn[*ref_position] =
        RenyiMap(Yn[*ref_position], parametros[*ref_position], j) +
        (epsilon & (*ref_H));
    *ref_H ^= Yn[*ref_position];
    *ref_position = Yn[*ref_position] & mask_numMapas;
    return ret_val;
}

/**
 * @brief Generates un numero aleatorio usando un ruleta de lookup tables, el
 * numero seleccionado en la tabla es reemplazado
 *
 * @param ref_roulete_position(puntero) Determina cual tabla en la ruleta se va
 * a utilizar
 * @param lu_table_position La posición en la tabla del numero que se selecciona
 * @param roulete Un arreglo de chaotic_lookup_tableS
 * @param parametros Una lista con los parametros de los mapas renyi
 * @param j El parametro de división para el mapa renyi
 * @param mask_numMapas Una mascara para hacer una operación modulo rápida y
 * seleccionar la casilla de la ruleta
 * @param epsilon Coeficiente de perturbación maxima
 * @param ref_H Coeficiente de perturbación
 * @return ull
 */
ull random_select_coupled_chaotic_map_lookuptable(ull *ref_roulete_position,
                                                  ull lu_table_position,
                                                  chaotic_lookup_table *roulete,
                                                  ull *parametros, ull j,
                                                  ull mask_numMapas,
                                                  ull epsilon, ull *ref_H) {
    chaotic_lookup_table *Yn = &roulete[*ref_roulete_position];
    const ull parametro = parametros[*ref_roulete_position];
    Yn->last_generated =
        RenyiMap(Yn->last_generated, parametro, j) + (epsilon & (*ref_H));
    *ref_H ^= Yn->last_generated;
    *ref_roulete_position = Yn->last_generated & mask_numMapas;
    const ull ret_val = Yn->lookup_table[lu_table_position];
    Yn->lookup_table[lu_table_position] = Yn->last_generated;
    return ret_val;
}

/**
 * @brief Generates un numero aleatorio usando un ruleta de lookup tables, el
 * numero seleccionado en la tabla es reemplazado, en lugar se seleccionar el
 * numero por posición selecciona un byte de la tabla y utiliza los 8 bytes a
 * partir del seleccionado.
 *
 * @param ref_roulete_position(puntero) Determina cual tabla en la ruleta se va
 * a utilizar
 * @param lu_table_position La posición en la tabla del numero que se selecciona
 * @param roulete Un arreglo de chaotic_lookup_tableS
 * @param parametros Una lista con los parametros de los mapas renyi
 * @param j El parametro de división para el mapa renyi
 * @param mask_numMapas Una mascara para hacer una operación modulo rápida y
 * seleccionar la casilla de la ruleta
 * @param epsilon Coeficiente de perturbación maxima
 * @param ref_H Coeficiente de perturbación
 * @return ull
 */

ull random_select_coupled_chaotic_map_lookuptable_byte(
    ull *ref_roulete_position, ull lu_table_position,
    chaotic_lookup_table *roulete, ull *parametros, ull renyi_j,
    ull mask_numMapas, ull epsilon, ull *ref_H) {
    chaotic_lookup_table *Yn = &roulete[*ref_roulete_position];
    const ull parametro = parametros[*ref_roulete_position];
    Yn->last_generated =
        RenyiMap(Yn->last_generated, parametro, renyi_j) + (epsilon & (*ref_H));
    *ref_H ^= Yn->last_generated;
    *ref_roulete_position = Yn->last_generated & mask_numMapas;
    const ull ret_val =
        *((ull *)(((unsigned char *)(Yn->lookup_table)) + lu_table_position));
    *((ull *)(((unsigned char *)(Yn->lookup_table)) + lu_table_position)) =
        Yn->last_generated;
    return ret_val;
}

/**
 * @brief Genera una secuencia de numeros aleatorios, para ello utiliza una
 * ruleta en la que cada casilla contiene una tabla de numeros pregenerados
 * utilizando mapas caoticos. Despues de generar la secuencia, un numero de dos
 * mapas contiguos para generar un numero adicional en la secuencia.
 * Cada numero usado es reemplazado por el mapa correspondiente
 *
 * @param ref_roulete_position (puntero) Determina cual tabla en la ruleta se va
 * a utilizar
 * @param roulete Un arreglo de chaotic_lookup_tableS
 * @param mask_numMapas Una mascara para hacer una operación modulo rápida y
 * seleccionar la casilla de la ruleta
 * @param mask_tablesize Una mascara para hacer una operación modulo rápida y
 * seleccionar la posición de la tabla
 * @param lu_table_position_index_list una lista de numerós usados para
 * seleccionar la posición de los numeros en la tabla
 * @param mask_lut_pos_indxsize Una mascara para hacer una operación modulo
 * rápida y evitar un desbordamiento en la tabla.
 * @param parametros Una lista con los parametros de los mapas renyi
 * @param j El parametro de división para el mapa renyi
 * @param epsilon Coeficiente de perturbación maxima
 * @param ref_H Coeficiente de perturbación
 * @param placeholder Arreglo donde se guardan los numeros generados
 * @param placeholder_size Tamaño del placeholder
 * @param number_of_generated_numbers (ret) Cantidad de numeros que se generaron
 * por la función
 * @param perturbation_size_auxiliar
 */
ull random_select_coupled_chaotic_map_lookuptable_horizontal_perturbation(
    chaotic_lookup_table *const roulete, ull *const ref_roulete_position,
    ull mask_numMapas, ull mask_tablesize, ull *const table_position,
    byte *const lu_table_position_index_list, ull mask_lut_pos_indxsize,
    ull *const parametros, ull j, ull epsilon, ull *const ref_H) {
    // Obtiene mascara para el offset de combinación de numeros
    const uint8_t u64_bit_size = (sizeof(uint64_t) << 3);
    const uint8_t mask_offset = (sizeof(uint64_t) << 3) - 1;
    // Obtiene lookup tables que se van a utilizar
    chaotic_lookup_table *Yn = &roulete[*ref_roulete_position];
    chaotic_lookup_table *Yn_plus1 =
        &roulete[((*ref_roulete_position) + 1) & mask_numMapas];
    // Obtiene la posición del numero aleatorio
    const uint64_t lu_table_position =
        lu_table_position_index_list[(*table_position)] & mask_tablesize;
    (*table_position) = ((*table_position) + 1) & mask_lut_pos_indxsize;
    // Obtiene el offset para combinar los numeros aleatorios
    const uint64_t offset =
        lu_table_position_index_list[(*table_position)] & mask_offset;
    (*table_position) = ((*table_position) + 1) & mask_lut_pos_indxsize;
    // Crea mascara para reemplazar el los bits usados
    const uint64_t mask_replace = (~(0ull)) >> offset;

    // Coloca los numeros que se van a usar en punteros
    uint64_t *const num_1 = &(Yn->lookup_table[lu_table_position]);
    uint64_t *const num_2 = &(Yn_plus1->lookup_table[lu_table_position]);

    // Genera el proximo numero aleatorio
    Yn->last_generated =
        RenyiMap(Yn->last_generated, parametros[*ref_roulete_position], j) +
        (epsilon & (*ref_H));
    // Actualiza la perturbacion
    *ref_H ^= Yn->last_generated;
    *ref_roulete_position = Yn->last_generated & mask_numMapas;
    const uint64_t ret_val =
        ((*num_1) & mask_replace) | ((*num_2) & (~mask_replace));
    *num_1 =
        ((*num_1) & (~mask_replace)) | (Yn->last_generated & (mask_replace));
    *num_2 =
        ((*num_2) & (mask_replace)) | (Yn->last_generated & (~mask_replace));
    return ret_val;
}

void cipher_data(unsigned char *plain_text, const unsigned char *cipher_value,
                 const ull cipher_size) {
    unsigned char *ptr_plain_text = plain_text;
    const unsigned char *ptr_cipher_value = cipher_value;
    for (ull _ = 0; _ < cipher_size;
         ++_, ++ptr_plain_text, ++ptr_cipher_value) {
        *ptr_plain_text ^= *ptr_cipher_value;
    }
}