#ifndef ENGINE_SOURCES_ECS_FILE_REGISTRY_H
#define ENGINE_SOURCES_ECS_FILE_REGISTRY_H

#include <Containers/DenseMap/DenseMap.h>
#include <ECS/Entity.h>
#include <ECS/Recycler.h>
#include <ECS/Containers/PoolGroup/PoolGroup.h>
#include <ECS/Containers/PoolGroup/PoolGroupInterface.h>
#include <ECS/Containers/SparseSet/SparseSet.h>
#include <ECS/Containers/Traits/PoolTraits.h>
#include <Types/Types.h>
#include <Types/Capabilities/Capabilities.h>
#include <Types/TypeInfo/TypeInfo.h>

#include <algorithm>
#include <memory>
#include <utility>

namespace egg::ECS
{
    template <ValidEntity EntityParameter, Types::ValidAllocator<EntityParameter> AllocatorParameter = std::allocator<EntityParameter>>
    class Registry final
    {
        using RegistryAllocatorTraits = std::allocator_traits<AllocatorParameter>;

        template <typename KeyType, typename MappedType>
        using DenseMapFor = egg::Containers::DenseMap<
            KeyType, MappedType, std::identity, std::equal_to<>,
            typename RegistryAllocatorTraits::template rebind_alloc<std::pair<const KeyType, MappedType>>
        >;

        using PoolTraitsType = Containers::PoolTraits<EntityParameter, AllocatorParameter>;

        using PoolBaseType = typename PoolTraitsType::SparseSetType;

        using RecyclerType = Recycler<EntityParameter, AllocatorParameter>;

        using PoolContainerType = DenseMapFor<Types::IDType, std::shared_ptr<PoolBaseType>>;
        using GroupContainerType = DenseMapFor<Types::IDType, std::shared_ptr<Containers::PoolGroupInterface>>;

        using EntityTraitsType = EntityTraits<EntityParameter>;

    public:
        using AllocatorType = AllocatorParameter;

        using EntityType = EntityParameter;
        using VersionType = typename EntityTraitsType::VersionType;

        template <typename ElementType>
        using PoolFor = typename PoolTraitsType::template LifecycleStorageFor<ElementType>;


        constexpr Registry() : Registry { AllocatorType {} }
        {
        }

        constexpr explicit Registry(const AllocatorType& Allocator)
            : Entities { Allocator },
              Pools { Allocator },
              Groups { Allocator }
        {
        }

        Registry(const Registry&) = delete;

        constexpr Registry(Registry&&) noexcept(
            std::is_nothrow_move_constructible_v<RecyclerType> &&
            std::is_nothrow_move_constructible_v<PoolContainerType> &&
            std::is_nothrow_move_constructible_v<GroupContainerType>
        ) = default;

        constexpr ~Registry() noexcept = default;

        Registry& operator=(const Registry&) = delete;

        constexpr Registry& operator=(Registry&&) noexcept(
            std::is_nothrow_move_assignable_v<RecyclerType> &&
            std::is_nothrow_move_assignable_v<PoolContainerType> &&
            std::is_nothrow_move_assignable_v<GroupContainerType>
        ) = default;

        constexpr friend void swap(Registry& Left, Registry& Right) noexcept
        {
            using std::swap;
            swap(Left.Entities, Right.Entities);
            swap(Left.Pools, Right.Pools);
            swap(Left.Groups, Right.Groups);
        }

        template <Types::Decayed ElementType>
        [[nodiscard]] constexpr PoolFor<ElementType>& GetPoolFor()
        {
            using PoolType = PoolFor<ElementType>;

            const auto [It, Inserted] { Pools.TryEmplace(GetHashFor<ElementType>()) };

            typename PoolContainerType::MappedType& PoolPointer { It->second };

            if (Inserted) PoolPointer = std::allocate_shared<PoolType>(GetAllocator(), *this, GetAllocator());

            return static_cast<PoolType&>(*PoolPointer);
        }

        template <Types::Decayed ElementType>
        [[nodiscard]] constexpr const PoolFor<ElementType>* GetPoolFor() const
        {
            if (const auto It { Pools.Find(GetHashFor<ElementType>()) };
                It != Pools.End())
            {
                return static_cast<const PoolFor<ElementType>*>(It->second.get());
            }

            return nullptr;
        }

        template <Types::Decayed ElementType>
        constexpr bool ResetPoolFor()
        {
            return Pools.Erase(GetHashFor<ElementType>());
        }

        [[nodiscard]] constexpr bool Valid(const EntityType Entity) const noexcept
        {
            return Entities.Valid(Entity);
        }

        [[nodiscard]] constexpr VersionType GetVersion(const EntityType Entity) const noexcept
        {
            return Entities.GetVersion(Entity);
        }

        [[nodiscard]] constexpr EntityType Create()
        {
            return Entities.Create();
        }

        constexpr VersionType Destroy(const EntityType Entity)
        {
            for (auto&& PoolPointer : Pools | std::views::values)
            {
                PoolPointer->Remove(Entity);
            }

            return Entities.Recycle(Entity);
        }

        constexpr void Clear()
        {
            for (auto&& PoolPointer : Pools | std::views::values)
            {
                PoolPointer->Clear();
            }

            Entities.Recycle();
        }

        template <typename... OwnTypes, typename... ViewTypes, typename... ExcludeTypes>
        [[nodiscard]] constexpr
        Containers::Group<OwnType<OwnTypes...>, ViewType<ViewTypes...>, ExcludeType<ExcludeTypes...>, EntityType, AllocatorType>
        Group(ViewType<ViewTypes...>  = ViewType {}, ExcludeType<ExcludeTypes...>  = ExcludeType {})
        {
            using GroupType = Containers::Group<
                OwnType<OwnTypes...>,
                ViewType<ViewTypes...>,
                ExcludeType<ExcludeTypes...>,
                EntityType,
                AllocatorType
            >;
            using PoolsType = typename GroupType::PoolsType;

            if constexpr (Types::InstanceOf<PoolsType, Containers::PoolGroup>)
            {
                if (auto It = Groups.Find(GetHashFor<PoolsType>()); It != Groups.End())
                {
                    return GroupType { static_cast<PoolsType&>(*It->second) };
                }

                const auto [It, Inserted] { Groups.TryEmplace(GetHashFor<PoolsType>()) };

                typename GroupContainerType::MappedType& GroupPointer { It->second };

                if (Inserted)
                {
                    if constexpr (sizeof...(OwnTypes))
                    {
                        GroupPointer = std::allocate_shared<PoolsType>(
                            GetAllocator(),
                            std::forward_as_tuple(
                                GetPoolFor<std::remove_const_t<OwnTypes>>()...,
                                GetPoolFor<std::remove_const_t<ViewTypes>>()...
                            ),
                            std::forward_as_tuple(
                                GetPoolFor<std::remove_const_t<ExcludeTypes>>()...
                            )
                        );
                    }
                    else
                    {
                        GroupPointer = std::allocate_shared<PoolsType>(
                            GetAllocator(),
                            std::forward_as_tuple(GetPoolFor<std::remove_const_t<ViewTypes>>()...),
                            std::forward_as_tuple(GetPoolFor<std::remove_const_t<ExcludeTypes>>()...),
                            GetAllocator()
                        );
                    }
                }

                return GroupType { static_cast<PoolsType&>(*GroupPointer) };
            }
            else
            {
                return GroupType { GetPoolFor<typename PoolsType::ElementType>() };
            }
        }

        [[nodiscard]] constexpr AllocatorType GetAllocator() const noexcept
        {
            return Entities.GetAllocator();
        }

    private:
        template <typename Type>
        [[nodiscard]] constexpr Types::IDType GetHashFor()
        {
            return Types::TypeInfo<Type>::template GetID<Registry>();
        }

        RecyclerType Entities;
        PoolContainerType Pools;
        GroupContainerType Groups;
    };
}

#endif // ENGINE_SOURCES_ECS_FILE_REGISTRY_H
