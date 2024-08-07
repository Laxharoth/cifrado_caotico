#include <stdio.h>

#include "ConfigStructure.c"
#include "ConfigStructure.h"
#include "chi_squared.c"
#include "roulete.c"
#include "roulete.h"

// NUMERO DE MAPAS SE PUEDE PASAR COMO ARGUMENTO DEL PROGRAMA

int main(int argc, char const *argv[]) {
    const size_t hidden_iterations = (size_t)6e3;
    const size_t total_iterations = (size_t)6e3;
    const double significance_level = 0.05;
    uint64_t *seed;
    Configuracion config;
    double *selection_distribution;
    int i = 0;
    int is_consistent = 0;
    int counts_as_uniform = 0;
    size_t is_consistent_from = 0;

    readConfigFile("config.txt", &config);
    if (argc >= 2) {
        config.num_maps = strtoul(argv[1], NULL, 0);
    } else {
        config.num_maps = 8;
    }

    seed = malloc(config.num_maps * sizeof(uint64_t) * 3);
    generate_seed(&config, seed);

    struct rouleteConfig roulete_config;
    initilizale_roulete(&config, &roulete_config, seed);

    selection_distribution = malloc(sizeof(double) * config.num_maps);
    memset(selection_distribution, 0, sizeof(double) * config.num_maps);

    // do some iterations without printing the result (printing is slow)
    for (i = 0; i < hidden_iterations; i++) {
        selection_distribution[roulete_config.roulette_selector %
                               config.num_maps]++;
        random_select_coupled_chaotic_map_lookuptable_bitoffset(
            &roulete_config);
        double p_value = chiIsUniform(selection_distribution, config.num_maps,
                                      significance_level);
        if (p_value <= significance_level) {
            is_consistent = 0;
        } else {
            counts_as_uniform++;
            if (!is_consistent) {
                is_consistent = 1;
                is_consistent_from = i + 1;
            }
        }
    }
    for (; i < total_iterations; i++) {
        selection_distribution[roulete_config.roulette_selector %
                               config.num_maps]++;
        random_select_coupled_chaotic_map_lookuptable_bitoffset(
            &roulete_config);
        double p_value = chiIsUniform(selection_distribution, config.num_maps,
                                      significance_level);
        if (p_value <= significance_level) {
            is_consistent = 0;
        } else {
            if (!is_consistent) {
                is_consistent = 1;
                is_consistent_from = i + 1;
            }
        }
        printf("iteration %07d is uniform?: %f, %c\n", i + 1, p_value,
               (p_value > significance_level) ? 'T' : 'F');
        ;
    }
    printf("num_maps: %d\n", config.num_maps);
    printf("significance_level: %f\n", significance_level);
    if (is_consistent)
        printf("Passes hypothesis test from: %d/%d\n", is_consistent_from,
               total_iterations);
    else
        printf("does not pass hypothesis test\n");
    printf("Uniform pass rate: %f\n",
           ((double)counts_as_uniform) / total_iterations);
    FILE *stream = fopen("distribution.txt", "w");
    for (int i = 0; i < config.num_maps; i++)
        fprintf(stream, "%f\n", selection_distribution[i]);
    fclose(stream);

    free(selection_distribution);
    free(seed);
    delete_roulete(&roulete_config);

    return 0;
}
