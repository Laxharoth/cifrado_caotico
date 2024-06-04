#include "ConfigStructure.h"
#include "chi_squared.c"
#include "roulete.h"
#include "stdio.h"

int main(int argc, char const *argv[]) {
    const size_t stream_size = (size_t)1e3;  // 1 MB

    Configuracion config;
    readConfigFile("config.txt", &config);
    struct rouleteConfig roulete_config;
    initilizale_roulete(&config, &roulete_config);

    double *selection_distribution = malloc(sizeof(double) * config.num_maps);
    memset(selection_distribution, 0, sizeof(double) * config.num_maps);
    int i = 0;
    for (i = 0; i < stream_size; i++) {
        selection_distribution[roulete_config.roulette_selector %
                               config.num_maps]++;
        random_select_coupled_chaotic_map_lookuptable(&roulete_config);
        printf("iteration %07d is uniform: %f\n", i,
               chiIsUniform(selection_distribution, config.num_maps, 0.05));
    }

    FILE *stream = fopen("distribution.txt", "w");
    for (int i = 0; i < config.num_maps; i++)
        printf(" %f\n", selection_distribution[i]);
    fclose(stream);

    free(selection_distribution);
    delete_roulete(&roulete_config);

    return 0;
}
