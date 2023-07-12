#ifndef FN_C
#define FN_C
#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#define min(a, b) (((a) < (b)) ? (a) : (b))

#define Π 3.141592653589793

unsigned long long find_most_significant_bit(unsigned long long value) {
    unsigned long long msb;
    while (value > 1) {
        value >>= 1;
        msb++;
    }
    return msb;
}

unsigned long long sqrtull(unsigned long long a) {
    unsigned long long min = 0;
    unsigned long long max = (1ull) << 32;

    while (1) {
        if (max <= 1 + min) return min;

        unsigned long long sqt = min + (max - min) / 2;
        unsigned long long sq = sqt * sqt;

        if (sq == a) return sqt;

        if (sq > a)
            max = sqt;
        else
            min = sqt;
    }
}

double logistic_map(double x, double μ) { return 4 * μ * x * (1 - x); }

double sine_map(double x, double μ) { return μ * sin(x * Π); }

double tent_map(double x, double μ) {
    return (x < 0.5) ? 2 * μ * x : 2 * μ * (1 - x);
}

typedef double (*chaotic_map)(double x, double μ);

double Γ(double x, double μ, double (*R)(double x, double μ),
         double (*S)(double x, double μ)) {
    const double a = R(x, μ);
    const double b = S(x, 1 - μ);
    const double c = S(b, μ);
    const double ans = (exp(a + b) + c);
    return ans - floor(ans);
};

double chaotic_map_walk(chaotic_map map, double x, double μ,
                        unsigned int distance) {
    while (distance-- > 0) {
        x = map(x, μ);
    }
    return x;
}

void chaotic_map_sample(chaotic_map map, double x, double μ, unsigned int size,
                        double *sample) {
    *sample = map(x, μ), size--;
    while (size-- > 0) {
        sample++;
        *sample = map(*(sample - 1), μ);
    }
}

inline long long exp_taylor(long long x) {
    long long result = 1;
    long long tmp = x;
    result += tmp;
    tmp = (x * tmp) >> 1;
    result += tmp;
    tmp *= x / 3;
    result += tmp;
    tmp = (x * tmp) >> 2;
    result += tmp;
    return result;
}

inline long long exp_base_2(unsigned int x) { return 1 << x; }

inline unsigned long long rotl64(unsigned long long value, unsigned int count) {
    const unsigned int mask = CHAR_BIT * sizeof(value) - 1;
    count &= mask;
    return (value << count) | (value >> (-count & mask));
}

inline unsigned long long rotr64(unsigned long long value, unsigned int count) {
    const unsigned int mask = CHAR_BIT * sizeof(value) - 1;
    count &= mask;
    return (value >> count) | (value << (-count & mask));
}

#define max_value (0xFFFFFFFFFFFFFFFF >> 1) - 1
#define RenyiMap(X, β, λ) (X * β) + (X >> λ)
#define LogisticMap(X, r) r *X *(max_value - X)

#define NUMBER_OF_CAHOTIC_MAPS 8

void renyi_array_generator(unsigned long long X[NUMBER_OF_CAHOTIC_MAPS],
                           const unsigned int β, const unsigned int λ) {
    typedef unsigned long long ull;
    typedef unsigned char byte;
    ull rn1, rn2, avr;
    rn1 = rn2 = 0;
    unsigned i = 0;
    ull *Xi;

    for (Xi = X, i = 0; i < 4; i++, Xi++) {
        *Xi = RenyiMap(*Xi, β, λ);
        rn1 ^= *Xi;
    }

    byte *rotation = (unsigned char *)(void *)&rn1;
    byte *bit_toggle = rotation + 4;

    for (Xi = X + 4, i = 0; i < 4; i++, Xi++, rotation++, bit_toggle++) {
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

unsigned long long renyi_array_combine_maps_with_mask_and_replace(
    unsigned long long X[NUMBER_OF_CAHOTIC_MAPS], const unsigned int β,
    const unsigned int λ) {
    //

    typedef unsigned long long ull;
    typedef unsigned char byte;
    const byte MOD_8_BIT_OPERATION_CONST = 0B00000111;
    const byte MOD_255_BIT_OPERATION_CONST = 0B11111111;
    const ull mask = 0x00000000FFFFFFFFULL;
    static byte index_selector = 0;

    const byte index_1 = index_selector,
               index_2 = (index_selector + 1) & MOD_8_BIT_OPERATION_CONST;
    const ull Y = (X[index_1] & mask) | (X[index_2] & ~mask);
    X[index_2] =
        RenyiMap((X[index_2] + (Y & MOD_255_BIT_OPERATION_CONST)), β, λ);

    index_selector = index_2;
    return Y;
}

unsigned long long renyi_array_random_byte_select_with_replace(
    unsigned long long X[NUMBER_OF_CAHOTIC_MAPS], const unsigned char β,
    const unsigned char λ) {
    typedef unsigned long long ull;
    typedef unsigned char byte;
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
unsigned long long logistic_renyi_map(const unsigned long long X,
                                      const unsigned int β,
                                      const unsigned int λ,
                                      const unsigned int r) {
    const unsigned long long Y = RenyiMap(X, β, λ);
    return LogisticMap(Y, r);
}
unsigned long long logistic_renyi_map_choice(const unsigned long long X,
                                             const unsigned int β,
                                             const unsigned int λ,
                                             const unsigned int r) {
    const unsigned long long Y = RenyiMap(X, β, λ);
    if (Y & 1)
        return Y;
    else
        return LogisticMap(Y, r);
}
#endif /* FN_C */
