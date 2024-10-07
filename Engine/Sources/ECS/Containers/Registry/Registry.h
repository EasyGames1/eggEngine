#ifndef ENGINE_SOURCES_ECS_CONTAINERS_REGISTRY_FILE_REGISTRY_H
#define ENGINE_SOURCES_ECS_CONTAINERS_REGISTRY_FILE_REGISTRY_H

#include "../Container.h"
#include "../../Entity.h"
#include "Containers/DenseMap/DenseMap.h"
#include "ECS/Containers/SparseSet/SparseSet.h"
#include "Type/TypeID.h"

#include <functional>

namespace egg::ECS::Containers
{
    template <ValidEntity EntityParameter, ValidAllocator<EntityParameter> AllocatorParameter = std::allocator<EntityParameter>>
    class Registry
    {
        using AllocatorTraits = AllocatorTraits<AllocatorParameter>;
        using StorageBase = SparseSet<EntityParameter, AllocatorParameter>;

        using PoolContainer = egg::Containers::DenseMap<
            Types::TypeID, StorageBase*,
            std::identity, std::equal_to<>,
            typename AllocatorTraits::template rebind_alloc<std::pair<const Types::TypeID, StorageBase*>>
        >;

    public:

    private:
    };
}

#endif // ENGINE_SOURCES_ECS_CONTAINERS_REGISTRY_FILE_REGISTRY_H
