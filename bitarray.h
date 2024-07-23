#ifndef BITARRAY_H
#define BITARRAY_H
#include <stdio.h>
#include <stdlib.h>

char ON(const char *bitarray, size_t bit);

void SET_ON(char *bitarray, size_t bit);

void SET_OFF(char *bitarray, size_t bit);

void TOGGLE(char *bitarray, size_t bit);

void readBitArrayFromFile(const char *filename, char *bitarray,
                          size_t max_size);

#endif  // BITARRAY_H