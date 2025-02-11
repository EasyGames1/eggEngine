#ifndef ENGINE_SOURCES_ECS_CONTAINERS_GROUP_INTERNAL_FILE_POOL_TRAITS_H
#define ENGINE_SOURCES_ECS_CONTAINERS_GROUP_INTERNAL_FILE_POOL_TRAITS_H

#include <ECS/Entity.h>
#include <ECS/Containers/Lifecycle/Lifecycle.h>
#include <ECS/Containers/SparseSet/SparseSet.h>
#include <ECS/Containers/Storage/Storage.h>
#include <Types/Capabilities/Capabilities.h>
#include <Types/Deduction/Deduction.h>

#include <tuple>
#include <type_traits>

namespace egg::ECS::Containers
{
    template <ValidEntity EntityParameter, Types::ValidAllocator<EntityParameter> AllocatorParameter = std::allocator<EntityParameter>>
    class PoolTraits
    {
        using PoolAllocatorTraits = std::allocator_traits<AllocatorParameter>;

    public:
        using EntityType = EntityParameter;
        using AllocatorType = AllocatorParameter;

        using SparseSetType = SparseSet<EntityType, AllocatorType>;

        using LifecycleSparseSetType = Lifecycle<SparseSetType>;

        template <typename ElementType>
        using StorageFor = Storage<
            std::remove_const_t<ElementType>,
            EntityType,
            typename PoolAllocatorTraits::template rebind_alloc<std::remove_const_t<ElementType>>
        >;

        template <typename ElementType>
        using LifecycleStorageFor = Lifecycle<StorageFor<ElementType>>;

        template <typename ElementType>
        using StorablePredicate = std::bool_constant<!OptimizableElement<ElementType, EntityType>>;

        template <typename... ElementTypes>
        using StorableTuple = Types::CombineTuples<
            std::tuple,
            std::tuple<EntityType>,
            Types::InstantiateTuple<
                std::add_lvalue_reference_t,
                Types::FilterTuple<
                    StorablePredicate,
                    std::tuple<ElementTypes...>
                >
            >
        >;
    };
}

#endif // ENGINE_SOURCES_ECS_CONTAINERS_GROUP_INTERNAL_FILE_POOL_TRAITS_H
