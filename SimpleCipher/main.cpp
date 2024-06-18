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
#include "random_generator.h"

#define NUM_MAPS 4

struct PGMImage;
PGMImage read_pgm(const std::string& filename);
void write_pgm(const std::string& filename, const PGMImage& image);

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

struct PGMImage {
    int width;
    int height;
    int max_val;
    std::vector<uint8_t> pixels;
};

PGMImage read_pgm(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("No se pudo abrir el archivo: " + filename);
    }

    std::string format;
    file >> format;
    if (format != "P5") {
        throw std::runtime_error("Formato PGM no soportado: " + format);
    }

    PGMImage image;
    file >> image.width >> image.height >> image.max_val;

    // Leer un solo carácter (salto de línea) después del encabezado
    file.get();

    image.pixels.resize(image.width * image.height);
    for (int i = 0; i < image.width * image.height; ++i) {
        image.pixels[i] = file.get();
    }

    file.close();
    return image;
}

void write_pgm(const std::string& filename, const PGMImage& image) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("No se pudo escribir el archivo: " + filename);
    }

    file << "P5\n"
         << image.width << " " << image.height << "\n"
         << image.max_val << "\n";

    for (int pixel : image.pixels) {
        file.put(pixel);
    }

    file.close();
}
