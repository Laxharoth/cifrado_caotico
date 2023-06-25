#include "include/externs.h"
#include "include/generators.h"
#include "include/genutils.h"
#include "include/utilities.h"

#include <openssl/sha.h>
#include <limits.h>

#define RENYIMAP(X, beta, lambda) (X*beta) + (X>>lambda)
void RenyiMap(){
	typedef unsigned long long ull;
    if ( ((epsilon = (BitSequence *) calloc(tp.n,sizeof(BitSequence))) == NULL) ) {
		printf("Insufficient memory available.\n");
		exit(1);
	}

    size_t chunk_size = sizeof(unsigned long long);  // Tamaño de cada fragmento a escribir

    ull X0 = 10;
    ull Xi = X0, beta1 = 15, lambda = 3;
	for (size_t i = 0; i < tp.numOfBitStreams; ++i) {
	    BitSequence *chunk = epsilon;
	    size_t remaining_bytes = tp.n;
		while(remaining_bytes > 0){
	        size_t chunk_bytes = (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;
			Xi = (
                RENYIMAP((Xi), beta1, lambda)
              ) >> 2 ;  // Generar valores aleatorios de 64 bits
			memcpy(chunk, &Xi, chunk_bytes);
	        chunk += chunk_bytes;
			remaining_bytes -= chunk_bytes;
		}
		nist_test_suite();
	}
	free(epsilon);
}

unsigned long long rotl64(unsigned long long value, unsigned int count) {
    const unsigned int mask = CHAR_BIT * sizeof(value) - 1;
    count &= mask;
    return (value << count) | (value >> (-count & mask));
}

unsigned long long RenyiArrayGenerate(unsigned long long X[8]) {
    typedef unsigned long long ull;
    typedef unsigned char byte;
    const int Β = 15, Λ = 3;
    ull rn1, rn2, avr;
    rn1 = rn2 = 0;
    unsigned i = 0;
    ull *Xi;

    for (Xi = X, i = 0; i < 4; i++, Xi++) {
        *Xi = RENYIMAP(*Xi, Β, Λ);
        rn1 += *Xi;
    }

    byte *rotation = (unsigned char *)(void *)&rn1;
    byte *bit_toggle = rotation + 4;

    for (Xi = X + 4, i = 0; i < 4; i++, Xi++, rotation++, bit_toggle++) {
        *Xi = RENYIMAP(*Xi, Β, Λ);
        *Xi = rotl64(*Xi, (*rotation) & 63);
        *Xi ^= 1 << ((*bit_toggle) & 63);
        rn2 += (*Xi)>>2;
    }
    avr = (rn1 + rn2) >> 3;
    for (Xi = X, i = 0; i < 8; i++, Xi++) {
        *Xi = ((*Xi * 3) + avr) >> 2;
    }
    return rn2;
}

void RenyiArray(){
    typedef unsigned long long ull;
    if ( ((epsilon = (BitSequence *) calloc(tp.n,sizeof(BitSequence))) == NULL) ) {
		printf("Insufficient memory available.\n");
		exit(1);
	}
    
    const ull SEED = 0xAC2ED40CAC2ED40C;
    unsigned char* seed = (unsigned char*)(&SEED);
    ull X[8];
    unsigned char* x = (unsigned char*)X;
    ull Yi;
    SHA512(seed, sizeof(ull), x);

    const size_t chunk_size =
        sizeof(ull);  // Tamaño de cada fragmento a escribir
    for (size_t i = 0; i < tp.numOfBitStreams; ++i) {
	    BitSequence *chunk = epsilon;
	    size_t remaining_bytes = tp.n;
		while(remaining_bytes > 0){
	        size_t chunk_bytes = (chunk_size < remaining_bytes) ? chunk_size : remaining_bytes;
			Yi = RenyiArrayGenerate(X) ;  // Generar valores aleatorios de 64 bits
			memcpy(chunk, &Yi, chunk_bytes);
	        chunk += chunk_bytes;
			remaining_bytes -= chunk_bytes;
		}
		nist_test_suite();
	}
}
void customRNG() {
    RenyiArray();
}