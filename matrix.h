#include <stdlib.h>

#include "list.h"

typedef struct {
    int *a;
    unsigned long height;
    unsigned long width;
} matrix;

typedef struct {
    int horizontal, vertical;
} move;

/**
 * Gets an element in the matrix
 * @param m the matrix
 * @param y the y coordinate
 * @param x the x coordinate
 * @return A pointer to the element of the matrix
 */
inline int *const matrix_get(matrix *m, unsigned y, unsigned x);
/**
 * Gets the pointer to an element in the matrix
 * @param m the matrix
 * @param y the y coordinate
 * @param x the x coordinate
 * @return A pointer to the element of the matrix
 */
matrix matrix_new(unsigned height, unsigned width);
/**
 * deallocate the memory in the matrix
 * @param m the matrix
 */
void matrix_free(matrix *m);
/**
 * Gets the sum of all elements in the matrix
 * @param m the matrix
 * @return The sum of the elements of the matrix
 */
long matrix_sum(matrix *m);
