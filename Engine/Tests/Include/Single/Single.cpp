#include "./Single.h"

std::size_t Random::Get(const std::size_t Min, const std::size_t Max)
{
    return std::uniform_int_distribution { Min, Max }(MersenneTwister);
}

std::random_device Random::Device {};
std::seed_seq Random::SeedSequence { Device(), Device(), Device(), Device(), Device(), Device(), Device(), Device() };
std::mt19937 Random::MersenneTwister { SeedSequence };
