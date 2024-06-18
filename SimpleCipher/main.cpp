#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <fstream>
#include <memory>
#include <random>
#include <string>
#include <vector>

#include "cipher.h"
#include "pgm_image.cpp"
#include "pgm_image.h"
#include "random_generator.h"

#define NUM_MAPS 4

int main(int argc, char const* argv[]) {
    // get parameters for the program
    if (argc < 3) {
        printf("requires \"input file\", \"output file\"  [seed]");
        exit(EXIT_FAILURE);
    }
    const std::string input_file = argv[1];
    const std::string output_file = argv[2];
    const uint32_t num_maps = NUM_MAPS;

    if (argc >= 3) {
        std::srand(std::stoul(argv[3]));
    } else {
        std::srand(time(NULL));
    }

    // initialize chaotic maps
    std::vector<uint64_t> rng_seed(num_maps * 3);
    uint64_t* parameters_r = rng_seed.data();
    uint64_t* states = parameters_r + num_maps;
    uint64_t* parameters_l = states + num_maps;

    for (auto&& i : rng_seed) {
        i = std::rand();
        i = i << 32 | std::rand();
    }
    for (size_t i = 0; i < num_maps; i++) {
        parameters_l[i] &= 63;
    }

    // read image to cipher
    PGMImage img = read_pgm(input_file);
    uint64_t size_to_padded_buffer =
        (uint64_t)ceil(((double)img.pixels.size()) / sizeof(uint64_t));

    // pad image data to cipher
    std::unique_ptr<uint64_t> padded_buffer(
        new uint64_t[size_to_padded_buffer]);
    memcpy(padded_buffer.get(), img.pixels.data(), img.pixels.size());

    // Generate random stream
    std::unique_ptr<uint64_t> random_buffer(
        new uint64_t[size_to_padded_buffer]);
    secuencial_rmg(parameters_r, parameters_l, states, num_maps,
                   random_buffer.get(), size_to_padded_buffer);

    // cipher image
    cipher(padded_buffer.get(), random_buffer.get(), size_to_padded_buffer);

    // return pixels to image
    memcpy(img.pixels.data(), padded_buffer.get(), img.pixels.size());

    // save image
    write_pgm(output_file, img);

    return 0;
}
