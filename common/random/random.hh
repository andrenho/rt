#ifndef RT_RANDOM_HH
#define RT_RANDOM_HH

#include <limits>
#include <random>

class Random {
public:
    Random();
    explicit Random(size_t seed);

    [[nodiscard]] size_t seed() const { return seed_; }

    [[nodiscard]] int   next_int(int min=std::numeric_limits<int>::min(), int max=std::numeric_limits<int>::max());
    [[nodiscard]] float next_float(float min=std::numeric_limits<float>::min(), float max=std::numeric_limits<float>::max());

    [[nodiscard]] int   next_unit(int max=std::numeric_limits<int>::max()) { return next_int(0, max); }
    [[nodiscard]] int   next_ufloat(int max=std::numeric_limits<float>::max()) { return next_float(0.f, max); }

    [[nodiscard]] uint8_t next_uint8();

    std::mt19937& rng() { return rng_; }

    static size_t random_seed();

private:
    size_t seed_;
    std::mt19937 rng_;

    std::uniform_int_distribution<int> ui_;
    std::uniform_real_distribution<float> uf_;
    std::uniform_int_distribution<uint8_t> u8_;
};

#endif //RT_RANDOM_HH
