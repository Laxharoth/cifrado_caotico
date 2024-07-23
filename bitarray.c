#include "bitarray.h"

char ON(const char *bitarray, size_t bit) {
    return bitarray[bit >> 3] & (1 << (bit & 7));
}

void SET_ON(char *bitarray, size_t bit) {
    bitarray[bit >> 3] |= (1 << (bit & 7));
}

void SET_OFF(char *bitarray, size_t bit) {
    bitarray[bit >> 3] &= ~(1 << (bit & 7));
}

void TOGGLE(char *bitarray, size_t bit) {
    bitarray[bit >> 3] ^= (1 << (bit & 7));
}

void readBitArrayFromFile(const char *filename, char *bitarray,
                          size_t max_size) {
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        perror("Error al abrir el archivo");
        exit(EXIT_FAILURE);
    }

    size_t i = 0;
    int c;
    unsigned char one_mask = ((unsigned char)1);
    while ((c = fgetc(file)) != EOF && i < max_size) {
        if (c == '0') {
            SET_OFF(bitarray, i);
            i++;
        }
        if (c == '1') {
            SET_ON(bitarray, i);
            i++;
        }
    }

    fclose(file);
}