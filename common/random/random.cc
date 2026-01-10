#include "random.hh"

#include <ctime>

Random::Random()
    : Random(random_seed())
{
}

Random::Random(size_t seed)
    : seed_(seed), rng_(seed)
{

}

int Random::next_int(int min, int max)
{
    ui_.param(std::uniform_int_distribution<int>::param_type(min, max));
    return ui_(rng_);
}

float Random::next_float(float min, float max)
{
    uf_.param(std::uniform_real_distribution<float>::param_type(min, max));
    return uf_(rng_);
}

uint8_t Random::next_uint8()
{
    return u8_(rng_);
}

size_t Random::random_seed()
{
    return std::random_device()();
}
