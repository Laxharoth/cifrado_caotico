#ifndef FN_C
#define FN_C
#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#define min(a, b) (((a) < (b)) ? (a) : (b))
#ifndef NUMBER_OF_CAHOTIC_MAPS
#define NUMBER_OF_CAHOTIC_MAPS 8
#endif

typedef unsigned long long ull;
typedef unsigned char byte;

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

#define RenyiMap(X, β, λ) (X * β) + (X >> λ)
#define LogisticMap(X, r) r *X * (1.0 - X)
#define LogisticMapInt(X, r, t) r *X *(t - X)

/**Rotacion izquierda circular*/
inline ull rotl64(ull value, unsigned int count) {
    const unsigned int mask = CHAR_BIT * sizeof(value) - 1;
    count &= mask;
    return (value << count) | (value >> (-count & mask));
}
/**Rotacion derecha circular*/
inline ull rotr64(ull value, unsigned int count) {
    const unsigned int mask = CHAR_BIT * sizeof(value) - 1;
    count &= mask;
    return (value >> count) | (value << (-count & mask));
}

void renyi_array_generator(ull X[NUMBER_OF_CAHOTIC_MAPS], const unsigned int β,
                           const unsigned int λ) {
    ull rn1, rn2, avr;
    rn1 = rn2 = 0;
    unsigned i = 0;
    ull *Xi;
    const int half = NUMBER_OF_CAHOTIC_MAPS / 2;

    for (Xi = X, i = 0; i < half; i++, Xi++) {
        *Xi = RenyiMap(*Xi, β, λ);
        rn1 ^= *Xi;
    }

    byte *rotation = (byte *)(void *)&rn1;
    byte *bit_toggle = rotation + 4;

    for (Xi = X + half, i = half; i < NUMBER_OF_CAHOTIC_MAPS;
         i++, Xi++, rotation++, bit_toggle++) {
        *Xi = RenyiMap(*Xi, β, λ);
        *Xi = rotl64(*Xi, (*rotation) & 63);
        *Xi ^= 1 << ((*bit_toggle) & 63);
        rn2 ^= *Xi;
    }
    avr = (rn1 ^ rn2);
    for (Xi = X, i = 0; i < NUMBER_OF_CAHOTIC_MAPS; i++, Xi++) {
        *Xi = (*Xi) + (avr & 0b11111111);
    }
}

ull renyi_array_combine_maps_with_mask_and_replace(
    ull X[NUMBER_OF_CAHOTIC_MAPS], const unsigned int β, const unsigned int λ) {
    //

    const byte MOD_8_BIT_OPERATION_CONST = 0B00000111;
    const ull mask = 0x00000000FFFFFFFFULL;
    static byte index_selector = 0;

    const byte index_1 = index_selector,
               index_2 = (index_selector + 1) & MOD_8_BIT_OPERATION_CONST;
    const ull Y = (X[index_1] & mask) | (X[index_2] & ~mask);
    X[index_1] = RenyiMap(Y, β, λ);

    index_selector = index_2;
    return Y;
}

ull renyi_array_random_byte_select_with_replace(ull X[NUMBER_OF_CAHOTIC_MAPS],
                                                const byte β, const byte λ) {
    static byte index_selector = 0;
    const byte MOD_64_BIT_OPERATION_CONST = 0B00111111;
    ull Y;
    byte *ptr_YasBytes = (byte *)(void *)&Y;
    byte *ptr_XasBytes = (byte *)(void *)X;
    Y = *((ull *)(ptr_XasBytes + index_selector));
    index_selector = (*ptr_YasBytes & MOD_64_BIT_OPERATION_CONST);
    index_selector = min(index_selector, 56);
    *((ull *)(ptr_XasBytes + index_selector)) = RenyiMap(Y, β, λ);
    return Y;
}

inline ull logistic_renyi(const ull x, const ull β, const ull λ, const ull r,
                          const ull t) {
    const ull renyi = RenyiMap(x, β, λ);
    return LogisticMapInt(renyi, r, t);
}

inline ull logistic_renyi_with_cycle(const ull x, ull *const _y, const ull β,
                                     const ull λ, const ull r, const ull t,
                                     const ull size) {
    ull *y = _y;
    *y = RenyiMap(x, β, λ);
    for (ull i = 1; i < size; ++i) {
        *(++y) = LogisticMapInt(*y, r, t) + 0xFFFF;
    }
    return *_y;
}

inline ull logistic_renyi_with_random_cycle(const ull x, ull *y, const ull β,
                                            const ull λ, const ull r,
                                            const ull t, const ull mask) {
    const ull size = (x & mask) | 1;
    *y = RenyiMap(x, β, λ);
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

struct _chaotic_lookup_table {
    ull *lookup_table;
    ull lu_table_mask;
    ull lu_table_size;
    ull last_generated;
};
typedef struct _chaotic_lookup_table chaotic_lookup_table;

ull random_select_coupled_chaotic_map_lookuptable(ull *ref_roulete_position,
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
    const ull lu_table_position = Yn->last_generated & Yn->lu_table_mask;
    const ull ret_val = Yn->lookup_table[lu_table_position];
    Yn->lookup_table[lu_table_position] = Yn->last_generated;
    return ret_val;
}

ull random_select_coupled_chaotic_map_lookuptable_byte(
    ull *ref_roulete_position, chaotic_lookup_table *roulete, ull *parametros,
    ull j, ull mask_numMapas, ull epsilon, ull *ref_H) {
    chaotic_lookup_table *Yn = &roulete[*ref_roulete_position];
    const ull parametro = parametros[*ref_roulete_position];
    Yn->last_generated =
        RenyiMap(Yn->last_generated, parametro, j) + (epsilon & (*ref_H));
    *ref_H ^= Yn->last_generated;
    *ref_roulete_position = Yn->last_generated & mask_numMapas;
    const ull lu_table_position = Yn->last_generated & Yn->lu_table_mask;
    const ull ret_val =
        *((ull *)(((unsigned char *)(Yn->lookup_table)) + lu_table_position));
    *((ull *)(((unsigned char *)(Yn->lookup_table)) + lu_table_position)) =
        Yn->last_generated;
    return ret_val;
}

#endif /* FN_C */
