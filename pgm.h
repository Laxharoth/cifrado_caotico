/**
 * @file pgm.h
 * @brief extracted from
 * "https://ugurkoltuk.wordpress.com/2010/03/04/an-extreme-simple-pgm-io-api/"
 */
#ifndef PGM_H
#define PGM_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HI(num) (((num)&0x0000FF00) << 8)
#define LO(num) ((num)&0x000000FF)

typedef struct _PGMData {
    int row;
    int col;
    int max_gray;
    int **matrix;
} PGMData;

int **allocate_dynamic_matrix(int row, int col);

void deallocate_dynamic_matrix(int **matrix, int row);

void SkipComments(FILE *fp);

/*for reading, Also allocate matrix*/
PGMData *readPGM(const char *file_name, PGMData *data);

/*and for writing, Also de-allocate matrix*/
void writePGM(const char *filename, const PGMData *data);

#endif  // PGM_H