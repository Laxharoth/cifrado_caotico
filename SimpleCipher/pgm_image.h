#ifndef PGM_IMAGE_H
#define PGM_IMAGE_H
#include <fstream>
#include <memory>
#include <string>
#include <vector>

struct PGMImage {
    int width;
    int height;
    int max_val;
    std::vector<uint8_t> pixels;
};

PGMImage read_pgm(const std::string& filename);
void write_pgm(const std::string& filename, const PGMImage& image);
#endif /* PGM_IMAGE_H */
