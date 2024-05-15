#include <mpi.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "ConfigStructure.c"
#include "ConfigStructure.h"
#include "MasterSlave.cpp"
#include "cipher.c"
#include "cipher.h"
#include "random_value_timed_iterator.cpp"
#include "roulete.c"
#include "roulete.h"

#define TIME_SECONDS 36000ULL

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    // inicializar constantes
    const size_t stream_size = 32;  // 1 MB
    const size_t stream_size_64 = stream_size / sizeof(uint64_t);
    const size_t required_random_numbers = stream_size * 2 + 1;

    const uint8_t hash = 114;
    uint64_t *const stream_start = (uint64_t *)malloc(stream_size);

    // Cargar generador de números aleatorios
    Configuracion config;
    readConfigFile("config.txt", &config);
    // sobreescribir el tamaño del archivo por el numero deseado de números
    // aleatorios
    const size_t random_buffer_size_64 = required_random_numbers;
    config.file_size = stream_size;
    uint64_t *const random_buffer = (uint64_t *)malloc(config.file_size);
    struct rouleteConfig roulete_config;
    initilizale_roulete(&config, &roulete_config);
    roulete_generator(random_buffer, &config, &roulete_config);

    uint64_t aux_renyi_r = config.r;
    uint64_t aux_renyi_j = config.lambda;

    uint8_t hash_cmp = hash;

    cipher(stream_start, random_buffer, stream_size_64, random_buffer_size_64,
           0, &hash_cmp, aux_renyi_r, aux_renyi_j);

    int rank{};
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        VST::RandomTimedIterator start(stream_size_64);
        VST::RandomTimedIterator end(stream_size_64, TIME_SECONDS);
        std::vector<uint64_t> colition_counter(2, 0);
        auto extract = [](const std::vector<uint64_t> &v) {
            return (void *)v.data();
        };
        auto insert = [](void *buff, bool &r) { memcpy(&r, buff, 1); };
        auto treat = [](bool &res, std::vector<uint64_t> &out) {
            out[0] += res;
            out[1]++;
        };
        zey::master_variable_task<std::vector<uint64_t>, bool,
                                  std::vector<uint64_t>,
                                  VST::RandomTimedIterator>(
            MPI_COMM_WORLD, start, end, colition_counter, extract, insert,
            treat, MPI_UNSIGNED_LONG_LONG, 4, MPI_C_BOOL, 1, 1);
        double colition_probability =
            colition_counter[0] / (double)colition_counter[1];
        printf("colition_probability: %lu / %lu = %0.4f\n", colition_counter[0], colition_counter[1], colition_probability);
        zey::master_end_slaves(MPI_COMM_WORLD);
    } else {
        uint64_t *const stream_dirt = (uint64_t *)malloc(stream_size);
        auto labor = [&](uint64_t *task) {
            for (size_t i = 0; i < stream_size_64; i++)
                stream_dirt[i] ^= stream_start[i];
            decipher(stream_dirt, random_buffer, stream_size_64,
                     random_buffer_size_64, 0, &hash_cmp, aux_renyi_r,
                     aux_renyi_j);
            return hash_cmp == hash;
        };
        auto extract = [](bool &value) { return (void *)&value; };
        auto insert = [stream_size_64](void *data, uint64_t *num) {
            memcpy(data, num, sizeof(uint64_t) * stream_size_64);
        };
        zey::slave<uint64_t[4], bool>(MPI_COMM_WORLD, labor, insert, extract,
                                     MPI_UNSIGNED_LONG_LONG, stream_size_64,
                                     MPI_C_BOOL, 1, stream_size);
    }

    free(random_buffer);
    free(stream_start);

    MPI_Finalize();
    return 0;
}
