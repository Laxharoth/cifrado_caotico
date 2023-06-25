#include "matrix.h"

inline int *const matrix_get(matrix *m, unsigned y, unsigned x) {
    return m->a[y * m->width + x];
}

inline int *const matrix_get_submatrix(matrix *m, unsigned y, unsigned x,
                                       unsigned top, unsigned left) {
    return m->a[(y + top) * m->width + x + left];
}

inline const int *const matrix_get_submatrix(matrix *m, unsigned y, unsigned x,
                                             unsigned top, unsigned left) {
    return m->a[(y + top) * m->width + x + left];
}

matrix matrix_new(unsigned height, unsigned width) {
    matrix c = {malloc(height * width * sizeof(int)), height, width};
    return c;
}

void circular_template(matrix *m) {
    move move_sequence[] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
    unsigned sequence_iter = 0;
    for (unsigned i = 0; i <= m->height / 2; i++) {
        move change_direction_rule_sequence[] = {{i, m->width - i},
                                                 {m->height - i, m->width - i},
                                                 {m->height - i, i},
                                                 {i, i}};
        // start
        unsigned y = i, x = i;
        while (1) {
            // access value
            matrix_get_submatrix(m, y, x, i, i);
            x += move_sequence[sequence_iter].horizontal;
            y += move_sequence[sequence_iter].vertical;
        }
    }
}

void matrix_free(matrix *m) { free(m->a); }

long matrix_sum(matrix *m) {
    unsigned i, j;
    long sum = 0;
    for (i = 0; i < m->height; ++i) {
        for (j = 0; j < m->width; ++j) {
            sum += *matrix_get(m, i, j);
        }
    }
    return sum;
}
