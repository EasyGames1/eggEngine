#ifndef ENGINE_TESTS_FILE_SINGLE_H
#define ENGINE_TESTS_FILE_SINGLE_H

#include "../Sources/ECS/Entity.h"
#include "../Sources/ECS/Traits/EntityTraits.h"

using EntityType = egg::ECS::Entity;

using EntityTraitsType = egg::ECS::EntityTraits<EntityType>;

constexpr std::size_t IterationsCount { 3u };
static_assert(IterationsCount >= 3u, "Iterations count is too small");
static_assert(IterationsCount < EntityTraitsType::VersionMask / 2u, "Iterations count causes a version overflow");
static_assert(IterationsCount < EntityTraitsType::EntityMask / 2u, "Iterations count causes an entity overflow");

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

#endif // ENGINE_TESTS_FILE_SINGLE_H
