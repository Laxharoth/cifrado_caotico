#include "pgm_image.h"

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