#ifndef FN_C
#define FN_C
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define Π 3.141592653589793
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

#define RenyiMap(X, β, λ) (X * β) + (X >> λ)

unsigned long long renyi_array_generator(unsigned long long X[8], const unsigned int Β, const unsigned int Λ) {
    typedef unsigned long long ull;
    typedef unsigned char byte;
    ull rn1, rn2, avr;
    rn1 = rn2 = 0;
    unsigned i = 0;
    ull *Xi;

    for (Xi = X, i = 0; i < 4; i++, Xi++) {
        *Xi = RenyiMap(*Xi, Β, Λ);
        rn1 ^= *Xi;
    }

    byte *rotation = (unsigned char *)(void *)&rn1;
    byte *bit_toggle = rotation + 4;

    for (Xi = X + 4, i = 0; i < 4; i++, Xi++, rotation++, bit_toggle++) {
        *Xi = RenyiMap(*Xi, Β, Λ);
        *Xi = rotl64(*Xi, (*rotation) & 63);
        *Xi ^= 1 << ((*bit_toggle) & 63);
        rn2 ^= (*Xi)>>2;
    }
    avr = (rn1 ^ rn2);
    for (Xi = X, i = 0; i < 8; i++, Xi++) {
        *Xi = ((*Xi * 3) + avr) >> 2;
    }
    return rn2;
}

unsigned long long renyi_array_2(unsigned long long X[10], const unsigned int Β, const unsigned int Λ){
    typedef unsigned long long ull;
    typedef unsigned char byte;
    ull rn1, rn2, avr;
    rn1 = rn2 = 0;
    unsigned i = 0;
    ull *Xi;

    for (Xi = X, i = 0; i < 4; i++, Xi++) {
        *Xi = RenyiMap(*Xi, Β, Λ);
        rn1 += *Xi;
    }
    
    byte *selector1 = (byte*)(Xi), *selector2 = (byte*)(Xi + 1);
    byte index_1, index_2;
    selector1++, selector2++;

    for(unsigned i = 0; i < 6; i++) {
        index_1 = *selector1++;
        index_2 = *selector2++;
        index_1 = index_1 & 7;
        index_2 = index_2 & 7;

        rn1 += Xi[2 + index_1];
        rn2 += Xi[2 + index_2];
    }
    avr = (rn1 + rn2)>>1;
    for (Xi = X, i = 0; i < 8; i++, Xi++) {
        *Xi = ((*Xi * 3) + avr) >> 2;
    }
    return rn1^rn2;
}
#endif /* FN_C */
