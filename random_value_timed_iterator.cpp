#include <chrono>
#include <iostream>
#include <iterator>
#include <random>
#include <vector>

#include "time_measure.h"

namespace VST {
typedef uint64_t vst_block_size;
typedef std::vector<vst_block_size> seq;
class RandomTimedIterator {
   private:
    seq sequence;
    size_t iteration;
    std::random_device rd;
    std::mt19937_64 gen;
    std::uniform_int_distribution<uint64_t> dis;
    std::chrono::time_point<std::chrono::system_clock> time;

   public:
    // Constructor
    RandomTimedIterator(size_t total_sequence_size)
        : sequence(total_sequence_size, 0),
          iteration(1),
          rd(),
          gen(rd()),
          dis(),
          time() {
        for (auto& it : sequence) it = dis(gen);
        time = std::chrono::system_clock::now();
    }
    RandomTimedIterator(size_t total_sequence_size, size_t future_secs)
        : sequence(total_sequence_size, 0),
          iteration(1),
          rd(),
          gen(rd()),
          dis(),
          time() {
        for (auto& it : sequence) it = dis(gen);
        time = std::chrono::system_clock::now() +
               std::chrono::seconds(future_secs);
    }

    // Operadores de incremento
    RandomTimedIterator& operator++() {
        for (auto& it : sequence) it = dis(gen);
        iteration++;
        time = std::chrono::system_clock::now();
        return *this;
    }

    bool operator!=(const RandomTimedIterator& other) const {
        return this->time < other.time;
    }

    bool operator<=(const RandomTimedIterator& other) {
        return this->time <= other.time;
    }
    bool operator<(const RandomTimedIterator& other) {
        return this->time < other.time;
    }

    // Operador de indirección
    const seq& operator*() const { return sequence; }

    size_t get_iterations() const { return iteration; }
};
}  // namespace VST

// int main() {
//     VST::RandomTimedIterator start(1);
//     VST::RandomTimedIterator end(1, 10);

//     for (; start < end; ++start)
//         ;
//     printf("Finished %llu operations", start.get_iterations());
//     return 0;
// }