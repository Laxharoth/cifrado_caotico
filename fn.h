#ifndef FN_H
#define FN_H
#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#define min(a, b) (((a) < (b)) ? (a) : (b))
#ifndef NUMBER_OF_CAHOTIC_MAPS
#define NUMBER_OF_CAHOTIC_MAPS 8
#endif

#define RenyiMap(X, beta, renyi_lambda) (X * beta) + (X >> renyi_lambda)
#define LogisticMap(X, r) r *X * (1.0 - X)
#define LogisticMapInt(X, r, t) r *X *(t - X)
#define MAX_ULL (~(0ull))

typedef u_int64_t ull;
typedef u_int32_t uint;
typedef unsigned char byte;

ull sqrtull(ull a);

ull countBitsSet(ull num);

/**Rotacion izquierda circular*/
ull rotl64(ull value, unsigned int count);
/**Rotacion derecha circular*/
ull rotr64(ull value, unsigned int count);

void renyi_array_generator(ull *X, const unsigned int beta,
                           const unsigned int renyi_lambda, ull num_maps);

ull renyi_array_combine_maps_with_mask_and_replace(
    ull *X, const unsigned int beta, const unsigned int renyi_lambda,
    ull size_mask);

ull renyi_array_random_byte_select_with_replace(ull *X, const byte beta,
                                                const byte renyi_lambda,
                                                ull size_mask);

ull logistic_renyi(const ull x, const ull beta, const ull renyi_lambda,
                   const ull r, const ull t);

ull logistic_renyi_with_cycle(const ull x, ull *const _y, const ull beta,
                              const ull renyi_lambda, const ull r, const ull t,
                              const ull size);

ull logistic_renyi_with_random_cycle(const ull x, ull *y, const ull beta,
                                     const ull renyi_lambda, const ull r,
                                     const ull t, const ull mask);

ull logistic_generalized(ull x, ull h, ull k, ull factor_n);

void SecureReal_TimeChaoticPartialEncryptionGenerator(ull *Yn, ull *parametros,
                                                      ull j, ull epsilon,
                                                      ull *H, ull numMapas);

ull random_select_coupled_chaotic_map(ull *ref_position, ull *Yn,
                                      ull *parametros, ull j,
                                      ull mask_numMapas);

ull random_select_coupled_chaotic_map_with_perturbation(ull *ref_position,
                                                        ull *Yn,
                                                        ull *parametros, ull j,
                                                        ull epsilon, ull *ref_H,
                                                        ull mask_numMapas);

struct _chaotic_lookup_table {
    ull *lookup_table;
    ull lu_table_mask;
    ull lu_table_size;
    ull last_generated;
};
typedef struct _chaotic_lookup_table chaotic_lookup_table;

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
                                                  ull epsilon, ull *ref_H);

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
    ull mask_numMapas, ull epsilon, ull *ref_H);

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
    ull *const number_of_generated_numbers, ull perturbation_size_auxiliar);
#endif /* FN_H */
