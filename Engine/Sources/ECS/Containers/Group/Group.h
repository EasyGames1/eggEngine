#ifndef ENGINE_SOURCES_ECS_CONTAINERS_GROUP_FILE_GROUP_H
#define ENGINE_SOURCES_ECS_CONTAINERS_GROUP_FILE_GROUP_H

#include "./Internal/GroupIterator.h"
#include "./Internal/PoolTraits.h"

#include <ECS/Entity.h>
#include <ECS/Ownership.h>
#include <ECS/Containers/Container.h>
#include <ECS/Containers/Lifecycle.h>
#include <ECS/Containers/PoolGroup/PoolGroup.h>
#include <Types/Capabilities/Capabilities.h>
#include <Types/Deduction/Deduction.h>

#include <concepts>

namespace egg::ECS::Containers
{
    template <
        Types::InstanceOf<OwnType>, Types::InstanceOf<ViewType>, Types::InstanceOf<ExcludeType>,
        ValidEntity EntityParameter, Types::ValidAllocator<EntityParameter> = std::allocator<EntityParameter>>
    class Group;


    template <typename... OwnParameters, typename... ViewParameters, typename... ExcludeParameters,
              ValidEntity EntityParameter, Types::ValidAllocator<EntityParameter> AllocatorParameter> requires
        Types::AllTupleUnique<Types::InstantiateTuple<
            std::remove_const_t,
            std::tuple<OwnParameters..., ViewParameters..., ExcludeParameters...>
        >> && (sizeof...(OwnParameters) + sizeof...(ViewParameters) != 0u) &&
        (sizeof...(OwnParameters) + sizeof...(ViewParameters) != 1u || sizeof...(ExcludeParameters) != 0u)
    class Group<OwnType<OwnParameters...>, ViewType<ViewParameters...>, ExcludeType<ExcludeParameters...>,
                EntityParameter, AllocatorParameter>
    {
        using TraitsType = Internal::PoolTraits<EntityParameter, AllocatorParameter>;

        template <typename ElementType>
        using PoolFor = typename TraitsType::template LifecycleStorageFor<ElementType>;

        template <typename EntitiesIterator>
        using IteratorFor = Internal::GroupIterator<
            EntitiesIterator,
            OwnType<OwnParameters...>,
            ViewType<ViewParameters...>,
            AllocatorParameter
        >;

    public:
        using PoolsType = PoolGroup<
            OwnType<PoolFor<OwnParameters>...>,
            ViewType<PoolFor<ViewParameters>...>,
            ExcludeType<PoolFor<ExcludeParameters>...>
        >;

        using AllocatorType = AllocatorParameter;
        using EntityType = EntityParameter;

        using EntitiesIterator = typename PoolsType::Iterator;
        using EntitiesReverseIterator = typename PoolsType::ReverseIterator;

        using EntitiesIterable = egg::Containers::IterableAdaptor<EntitiesIterator>;
        using EntitiesReverseIterable = egg::Containers::IterableAdaptor<EntitiesReverseIterator>;

        using Iterator = IteratorFor<EntitiesIterator>;
        using ReverseIterator = IteratorFor<EntitiesReverseIterator>;


        constexpr explicit Group(PoolsType& Pools) : Pools { Pools }
        {
        }

        [[nodiscard]] constexpr Iterator Begin() const noexcept
        {
            return Iterator { EntitiesBegin(), Pools.GetPools() };
        }

        [[nodiscard]] constexpr Iterator End() const noexcept
        {
            return Iterator { EntitiesEnd(), Pools.GetPools() };
        }

        [[nodiscard]] constexpr ReverseIterator ReverseBegin() const noexcept
        {
            return Iterator { EntitiesReverseBegin(), Pools.GetPools() };
        }

        [[nodiscard]] constexpr ReverseIterator ReverseEnd() const noexcept
        {
            return Iterator { EntitiesReverseEnd(), Pools.GetPools() };
        }

        [[nodiscard]] constexpr EntitiesIterable Entities() const noexcept
        {
            return EntitiesIterable { EntitiesBegin(), EntitiesEnd() };
        }

        [[nodiscard]] constexpr EntitiesReverseIterable EntitiesReverse() const noexcept
        {
            return EntitiesReverseIterable { EntitiesReverseBegin(), EntitiesReverseEnd() };
        }

        [[nodiscard]] constexpr EntitiesIterator EntitiesBegin() const noexcept
        {
            return Pools.Begin();
        }

        [[nodiscard]] constexpr EntitiesIterator EntitiesEnd() const noexcept
        {
            return Pools.End();
        }

        [[nodiscard]] constexpr EntitiesReverseIterator EntitiesReverseBegin() const noexcept
        {
            return Pools.ReverseBegin();
        }

        [[nodiscard]] constexpr EntitiesReverseIterator EntitiesReverseEnd() const noexcept
        {
            return Pools.ReverseEnd();
        }

        template <typename CallableType> requires
            Types::Applicable<CallableType, std::iter_reference_t<Iterator>> ||
            Types::Applicable<CallableType, Types::RemoveTupleType<EntityType, std::iter_reference_t<Iterator>>>
        constexpr void Each(CallableType Callable) const
        {
            for (const auto Arguments : *this)
            {
                if constexpr (Types::Applicable<CallableType, std::iter_reference_t<Iterator>>)
                {
                    std::apply(Callable, Arguments);
                }
                else
                {
                    std::apply([&Callable]<typename... ElementTypes>(EntityType, ElementTypes&&... Elements) constexpr
                    {
                        Callable(std::forward<ElementTypes>(Elements)...);
                    }, Arguments);
                }
            }
        }

        template <typename... ElementTypes> requires
            (Types::ContainedIn<
                ElementTypes,
                Types::ConstnessAs<ElementTypes, OwnParameters>...,
                Types::ConstnessAs<ElementTypes, ViewParameters>...
            > && ...) &&
            Types::AllUnique<std::remove_const_t<ElementTypes>...> &&
            (!OptimizableElement<ElementTypes, EntityType> || ...)
        [[nodiscard]] constexpr decltype(auto) Get(const EntityType Entity) const
        {
            return Pools.template Get<ElementTypes...>(Entity);
        }

        [[nodiscard]] constexpr Iterator Find(const EntityType Entity) const noexcept
        {
            return Iterator { Pools.Find(Entity), Pools.GetPools() };
        }

        [[nodiscard]] constexpr bool Contains(const EntityType Entity) const noexcept
        {
            return Pools.Contains(Entity);
        }

        [[nodiscard]] constexpr std::size_t GetSize() const noexcept
        {
            return Pools.GetSize();
        }

        [[nodiscard]] constexpr bool Empty() const noexcept
        {
            return Pools.Empty();
        }

        [[nodiscard]] constexpr std::size_t GetCapacity() const noexcept requires (sizeof...(OwnParameters) == 0u)
        {
            return Pools.GetCapacity();
        }

        template <typename... ElementTypes,
                  typename CompareType,
                  typename SortType = decltype(std::ranges::sort),
                  typename ProjectionType = std::identity> requires
            (Types::ContainedIn<
                ElementTypes,
                Types::ConstnessAs<ElementTypes, OwnParameters>...,
                Types::ConstnessAs<ElementTypes, ViewParameters>...
            > && ...) &&
            Types::AllUnique<std::remove_const_t<ElementTypes>...>
        constexpr void Sort(CompareType Compare, SortType Sort = SortType {}, ProjectionType Projection = ProjectionType {}) const
        {
            Pools.template Sort<ElementTypes...>(std::move(Compare), std::move(Sort), std::move(Projection));
        }

        template <typename IteratorType, std::sentinel_for<IteratorType> SentinelType>
        constexpr Iterator SortAs(IteratorType First, SentinelType Last) const
        {
            return Iterator { Pools.SortAs(std::move(First), std::move(Last)), Pools.GetPools() };
        }

        constexpr void ShrinkToFit() requires (sizeof...(OwnParameters) == 0u)
        {
            Pools.ShrinkToFit();
        }

    private:
        PoolsType& Pools;
    };


    template <typename... OwnParameters, typename... ViewParameters,
              ValidEntity EntityParameter, Types::ValidAllocator<EntityParameter> AllocatorParameter> requires
        Types::AllTupleUnique<Types::InstantiateTuple<
            std::remove_const_t,
            std::tuple<OwnParameters..., ViewParameters...>
        >> &&
        (sizeof...(OwnParameters) + sizeof...(ViewParameters) == 1u) &&
        (!OptimizableElement<Types::CommonTypeOf<OwnParameters..., ViewParameters...>, EntityParameter>)
    class Group<OwnType<OwnParameters...>, ViewType<ViewParameters...>, ExcludeType<>,
                EntityParameter, AllocatorParameter>
    {
        using TraitsType = Internal::PoolTraits<EntityParameter, AllocatorParameter>;
        using CommonType = CommonTypeOf<OwnParameters..., ViewParameters...>;

        template <typename Const, typename NonConst>
        using SelectConstness = std::conditional_t<std::is_const_v<CommonType>, Const, NonConst>;

    public:
        using ElementType = CommonType;
        using PoolsType = typename TraitsType::template LifecycleStorageFor<ElementType>;

        using AllocatorType = AllocatorParameter;
        using EntityType = EntityParameter;

        using EntitiesIterator = typename PoolsType::EntitiesIterator;
        using EntitiesReverseIterator = typename PoolsType::EntitiesReverseIterator;

        using EntitiesIterable = egg::Containers::IterableAdaptor<EntitiesIterator>;
        using EntitiesReverseIterable = egg::Containers::IterableAdaptor<EntitiesReverseIterator>;

        using Iterator = SelectConstness<
            typename PoolsType::EachConstIterator,
            typename PoolsType::EachIterator
        >;
        using ReverseIterator = SelectConstness<
            typename PoolsType::EachConstReverseIterator,
            typename PoolsType::EachReverseIterator
        >;


        constexpr explicit Group(PoolsType& Pool) : Pool { Pool }
        {
        }

        [[nodiscard]] constexpr Iterator Begin() const noexcept
        {
            return GetPool().EachBegin();
        }

        [[nodiscard]] constexpr Iterator End() const noexcept
        {
            return GetPool().EachEnd();
        }

        [[nodiscard]] constexpr ReverseIterator ReverseBegin() const noexcept
        {
            return GetPool().EachReverseBegin();
        }

        [[nodiscard]] constexpr ReverseIterator ReverseEnd() const noexcept
        {
            return GetPool().EachReverseEnd();
        }

        [[nodiscard]] constexpr EntitiesIterable Entities() const noexcept
        {
            return EntitiesIterable { EntitiesBegin(), EntitiesEnd() };
        }

        [[nodiscard]] constexpr EntitiesReverseIterable EntitiesReverse() const noexcept
        {
            return EntitiesReverseIterable { EntitiesReverseBegin(), EntitiesReverseEnd() };
        }

        [[nodiscard]] constexpr EntitiesIterator EntitiesBegin() const noexcept
        {
            return GetPool().Begin();
        }

        [[nodiscard]] constexpr EntitiesIterator EntitiesEnd() const noexcept
        {
            return GetPool().End();
        }

        [[nodiscard]] constexpr EntitiesReverseIterator EntitiesReverseBegin() const noexcept
        {
            return GetPool().ReverseBegin();
        }

        [[nodiscard]] constexpr EntitiesReverseIterator EntitiesReverseEnd() const noexcept
        {
            return GetPool().ReverseEnd();
        }

        template <typename CallableType> requires
            Types::Applicable<CallableType, std::iter_reference_t<Iterator>> ||
            Types::Applicable<CallableType, Types::RemoveTupleType<EntityType, std::iter_reference_t<Iterator>>>
        constexpr void Each(CallableType Callable) const
        {
            if constexpr (Types::Applicable<CallableType, std::iter_reference_t<Iterator>>)
            {
                for (const auto Arguments : *this)
                {
                    std::apply(Callable, Arguments);
                }
            }
            else
            {
                for (auto&& Argument : GetPool().Elements())
                {
                    std::invoke(Callable, std::forward<decltype(Argument)>(Argument));
                }
            }
        }

        template <typename Type = ElementType> requires std::same_as<Type, Types::ConstnessAs<Type, ElementType>>
        [[nodiscard]] constexpr Type& Get(const EntityType Entity) const
        {
            return GetPool().Get(Entity);
        }

        [[nodiscard]] constexpr Iterator Find(const EntityType Entity) const noexcept
        {
            return ToIterator(GetPool().Find(Entity));
        }

        [[nodiscard]] constexpr bool Contains(const EntityType Entity) const noexcept
        {
            return GetPool().Contains(Entity);
        }

        [[nodiscard]] constexpr std::size_t GetSize() const noexcept
        {
            return GetPool().GetSize();
        }

        [[nodiscard]] constexpr bool Empty() const noexcept
        {
            return GetPool().Empty();
        }

        [[nodiscard]] constexpr std::size_t GetCapacity() const noexcept
        {
            return GetPool().GetCapacity();
        }

        template <typename... Type,
                  typename CompareType,
                  typename SortType = decltype(std::ranges::sort),
                  typename ProjectionType = std::identity> requires
            (sizeof...(Type) <= 1u) &&
            (std::same_as<Type, Types::ConstnessAs<Type, ElementType>> && ...)
        constexpr void Sort(CompareType Compare, SortType Sort = SortType {}, ProjectionType Projection = ProjectionType {}) const
            requires (sizeof...(OwnParameters) == 1u)
        {
            Pool.Sort(std::move(Compare), std::move(Sort), GetProjection<Type...>(std::move(Projection)));
        }

        template <typename IteratorType, std::sentinel_for<IteratorType> SentinelType>
        constexpr Iterator SortAs(IteratorType First, SentinelType Last) const
            requires (sizeof...(OwnParameters) == 1u)
        {
            return ToIterator(Pool.SortAs(std::move(First), std::move(Last)));
        }

        constexpr void ShrinkToFit() requires (sizeof...(OwnParameters) == 1u)
        {
            GetPool().ShrinkToFit();
        }

    private:
        template <typename... Type, typename ProjectionType> requires
            (sizeof...(Type) <= 1u) &&
            (std::same_as<Type, Types::ConstnessAs<Type, ElementType>> && ...)
        [[nodiscard]] constexpr auto GetProjection(ProjectionType Projection) const noexcept
        {
            if constexpr (sizeof...(Type))
            {
                return [this, Projection = std::move(Projection)](const EntityType Entity) constexpr
                {
                    return std::invoke(Projection, Get<Type...>(Entity));
                };
            }
            else
            {
                return Projection;
            }
        }

        [[nodiscard]] constexpr Iterator ToIterator(const EntitiesIterator Other) const noexcept
        {
            return Iterator { Other, GetPool().ElementsEnd() - (Other.GetIndex() + 1u) };
        }

        [[nodiscard]] constexpr Types::ConstnessAs<ElementType, PoolsType>& GetPool() const noexcept
        {
            return Pool;
        }

        PoolsType& Pool;
    };

    template <typename... OwnParameters, typename... ViewParameters,
              ValidEntity EntityParameter, Types::ValidAllocator<EntityParameter> AllocatorParameter> requires
        Types::AllTupleUnique<Types::InstantiateTuple<
            std::remove_const_t,
            std::tuple<OwnParameters..., ViewParameters...>
        >> &&
        (sizeof...(OwnParameters) + sizeof...(ViewParameters) == 1u) &&
        OptimizableElement<Types::CommonTypeOf<OwnParameters..., ViewParameters...>, EntityParameter>
    class Group<OwnType<OwnParameters...>, ViewType<ViewParameters...>, ExcludeType<>,
                EntityParameter, AllocatorParameter>
    {
        using TraitsType = Internal::PoolTraits<EntityParameter, AllocatorParameter>;
        using CommonType = CommonTypeOf<OwnParameters..., ViewParameters...>;

        template <typename Const, typename NonConst>
        using SelectConstness = std::conditional_t<std::is_const_v<CommonType>, Const, NonConst>;

    public:
        using ElementType = CommonType;
        using PoolsType = typename TraitsType::template LifecycleStorageFor<ElementType>;

        using AllocatorType = AllocatorParameter;
        using EntityType = EntityParameter;

        using Iterator = SelectConstness<typename PoolsType::ConstIterator, typename PoolsType::Iterator>;
        using ReverseIterator = SelectConstness<typename PoolsType::ConstReverseIterator, typename PoolsType::ReverseIterator>;


        constexpr explicit Group(PoolsType& Pool) : Pool { Pool }
        {
        }

        [[nodiscard]] constexpr Iterator Begin() const noexcept
        {
            return GetPool().Begin();
        }

        [[nodiscard]] constexpr Iterator End() const noexcept
        {
            return GetPool().End();
        }

        [[nodiscard]] constexpr ReverseIterator ReverseBegin() const noexcept
        {
            return GetPool().ReverseBegin();
        }

        [[nodiscard]] constexpr ReverseIterator ReverseEnd() const noexcept
        {
            return GetPool().ReverseEnd();
        }

        template <std::invocable<std::iter_reference_t<Iterator>> CallableType>
        constexpr void Each(CallableType Callable) const
        {
            for (auto&& Argument : *this)
            {
                std::invoke(Callable, std::forward<decltype(Argument)>(Argument));
            }
        }

        [[nodiscard]] constexpr Iterator Find(const EntityType Entity) const noexcept
        {
            return GetPool().Find(Entity);
        }

        [[nodiscard]] constexpr bool Contains(const EntityType Entity) const noexcept
        {
            return GetPool().Contains(Entity);
        }

        [[nodiscard]] constexpr std::size_t GetSize() const noexcept
        {
            return GetPool().GetSize();
        }

        [[nodiscard]] constexpr bool Empty() const noexcept
        {
            return GetPool().Empty();
        }

        [[nodiscard]] constexpr std::size_t GetCapacity() const noexcept
        {
            return GetPool().GetCapacity();
        }

        template <typename CompareType, typename SortType = decltype(std::ranges::sort), typename ProjectionType = std::identity>
        constexpr void Sort(CompareType Compare, SortType Sort = SortType {}, ProjectionType Projection = ProjectionType {}) const
            requires (sizeof...(OwnParameters) == 1u)
        {
            Pool.Sort(std::move(Compare), std::move(Sort), std::move(Projection));
        }

        template <typename IteratorType, std::sentinel_for<IteratorType> SentinelType>
        constexpr Iterator SortAs(IteratorType First, SentinelType Last) const
            requires (sizeof...(OwnParameters) == 1u)
        {
            return Pool.SortAs(std::move(First), std::move(Last));
        }

        constexpr void ShrinkToFit() requires (sizeof...(OwnParameters) == 1u)
        {
            GetPool().ShrinkToFit();
        }

    private:
        [[nodiscard]] constexpr Types::ConstnessAs<ElementType, PoolsType>& GetPool() const noexcept
        {
            return Pool;
        }

        PoolsType& Pool;
    };
}

#endif // ENGINE_SOURCES_ECS_CONTAINERS_GROUP_FILE_GROUP_H
