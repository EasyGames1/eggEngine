#ifndef ENGINE_TESTS_FILE_SINGLE_H
#define ENGINE_TESTS_FILE_SINGLE_H

#include "ECS/Entity.h"
#include "ECS/Traits/EntityTraits.h"

#include <random>

using EntityType = egg::ECS::Entity;

using EntityTraitsType = egg::ECS::EntityTraits<EntityType>;

constexpr std::size_t IterationsCount { 3 };
static_assert(IterationsCount >= 3, "Iterations count is too small");
static_assert(IterationsCount < EntityTraitsType::VersionMask / 2u, "Iterations count causes a version overflow");
static_assert(IterationsCount < EntityTraitsType::EntityMask / 2u, "Iterations count causes a entity overflow");

inline std::size_t GetReversedIndex(const std::size_t Index)
{
    return IterationsCount - Index - 1u;
}

inline EntityType GetEntityAt(const std::size_t Index)
{
    return EntityTraitsType::Construct(Index, Index);
}

inline EntityType GetEntityAtReversed(const std::size_t Index)
{
    return GetEntityAt(GetReversedIndex(Index));
}

class Random
{
public:
    [[nodiscard]] static std::size_t Get(
        std::size_t Min = 0u,
        std::size_t Max = (std::numeric_limits<std::size_t>::max)()
    );

private:
    static std::random_device Device;
    static std::seed_seq SeedSequence;
    static std::mt19937 MersenneTwister;
};

#endif // ENGINE_TESTS_FILE_SINGLE_H
