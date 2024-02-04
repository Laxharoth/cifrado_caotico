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
void random_select_coupled_chaotic_map_lookuptable_horizontal_perturbation(
    ull *const ref_roulete_position, chaotic_lookup_table *const roulete,
    ull mask_numMapas, ull mask_tablesize,
    ull *const lu_table_position_index_list, ull mask_lut_pos_indxsize,
    ull *const parametros, ull j, ull epsilon, ull *const ref_H,
    ull *const placeholder, ull placeholder_size,
    ull *const number_of_generated_numbers, ull perturbation_size_auxiliar) {
    const ull ull_byte_size_mask = sizeof(ull) - 1;
    *number_of_generated_numbers =
        min(placeholder_size, 150 + perturbation_size_auxiliar);
    ull *ptr_placeholder = placeholder;
    ull lu_table_position_index = 0, lu_table_position;
    ull ull_byte_position_mask = 0;
    ull iter;

    for (iter = 0; iter < *number_of_generated_numbers - 1;
         ++iter, ++ptr_placeholder) {
        lu_table_position =
            lu_table_position_index_list[iter & mask_lut_pos_indxsize] &
            mask_tablesize;
        lu_table_position_index =
            lu_table_position_index + 1 & mask_lut_pos_indxsize;
        *ptr_placeholder = random_select_coupled_chaotic_map_lookuptable(
            ref_roulete_position, lu_table_position, roulete, parametros, j,
            mask_numMapas, epsilon, ref_H);
    }
    *ref_roulete_position =
        lu_table_position_index_list[iter++ & mask_lut_pos_indxsize] &
        mask_numMapas;
    lu_table_position_index =
        lu_table_position_index + 1 & mask_lut_pos_indxsize;
    lu_table_position =
        lu_table_position_index_list[iter++ & mask_lut_pos_indxsize] &
        mask_tablesize;
    lu_table_position_index =
        lu_table_position_index + 1 & mask_lut_pos_indxsize;
    ull_byte_position_mask =
        (~(0ull)) >>
        (lu_table_position_index_list[iter++ & mask_lut_pos_indxsize] &
         ull_byte_size_mask);

    ull replace_value = random_select_coupled_chaotic_map_lookuptable(
        ref_roulete_position, lu_table_position, roulete, parametros, j,
        mask_numMapas, epsilon, ref_H);

    ull *num1 = &roulete[*ref_roulete_position].lookup_table[lu_table_position];
    ull *num2 = &roulete[((*ref_roulete_position) + 1) & mask_numMapas]
                     .lookup_table[lu_table_position];
    *ptr_placeholder =
        (*num1 & ull_byte_position_mask) | (*num2 & ~ull_byte_position_mask);
    *num1 = (replace_value & ull_byte_position_mask) |
            (*num1 & +~ull_byte_position_mask);
    *num2 = (replace_value & ~ull_byte_position_mask) |
            (*num2 & +ull_byte_position_mask);
}
