#ifndef ENGINE_SOURCES_ECS_CONTAINERS_GROUP_FILE_GROUP_H
#define ENGINE_SOURCES_ECS_CONTAINERS_GROUP_FILE_GROUP_H

#include "./Internal/GroupIterator.h"
#include "./Internal/GroupViewer.h"

#include <Containers/IterableAdaptor.h>
#include <ECS/Ownership.h>
#include <ECS/Containers/Container.h>
#include <ECS/Containers/LifecycleContainer.h>
#include <Types/Capabilities/Capabilities.h>
#include <Types/Deduction/Deduction.h>

#include <algorithm>
#include <iterator>
#include <ranges>
#include <tuple>

namespace egg::ECS::Containers
{
    template <Types::InstanceOf<OwnType>, Types::InstanceOf<ViewType>, Types::InstanceOf<ExcludeType>>
    class Group;


    template <
        Types::InstanceOf<Lifecycle>... OwnParameters,
        Types::InstanceOf<Lifecycle>... ViewParameters,
        Types::InstanceOf<Lifecycle>... ExcludeParameters
    > requires
        (Types::InstanceOf<typename OwnParameters::ContainerType, Storage> && ...) &&
        (Types::InstanceOf<typename ViewParameters::ContainerType, Storage> && ...) &&
        (Types::InstanceOf<typename ExcludeParameters::ContainerType, Storage> && ...) &&
        Types::AllUnique<typename OwnParameters::ElementType...,
                         typename ViewParameters::ElementType...,
                         typename ExcludeParameters::ElementType...> &&
        Types::AllSame<typename OwnParameters::BaseType...,
                       typename ViewParameters::BaseType...,
                       typename ExcludeParameters::BaseType...> &&
        (sizeof...(OwnParameters) + sizeof...(ViewParameters) != 0u) &&
        (sizeof...(OwnParameters) + sizeof...(ViewParameters) != 1u || sizeof...(ExcludeParameters) != 0u)
    class Group<OwnType<OwnParameters...>, ViewType<ViewParameters...>, ExcludeType<ExcludeParameters...>>
    {
    public:
        using CommonType = CommonTypeOf<
            typename OwnParameters::BaseType...,
            typename ViewParameters::BaseType...,
            typename ExcludeParameters::BaseType...
        >;
        using CommonAllocator = typename CommonType::AllocatorType;

        using CommonIterator = typename CommonType::Iterator;
        using CommonReverseIterator = typename CommonType::ReverseIterator;

        using CommonIterable = egg::Containers::IterableAdaptor<CommonIterator>;
        using CommonReverseIterable = egg::Containers::IterableAdaptor<CommonReverseIterator>;

        using EntityType = typename CommonType::EntityType;

        using Iterator = Internal::GroupIterator<CommonIterator, OwnType<OwnParameters...>, ViewType<ViewParameters...>>;
        using ReverseIterator = Internal::GroupIterator<CommonReverseIterator, OwnType<OwnParameters...>, ViewType<ViewParameters...>>;


        constexpr Group(const std::tuple<OwnParameters&..., ViewParameters&...> Pools, const std::tuple<ExcludeParameters&...> Filters)
            : Viewer {
                  Pools, Filters, *this,
                  Events::ConnectionArgument<&Group::PushOnConstruct>,
                  Events::ConnectionArgument<&Group::PushOnDestroy>,
                  Events::ConnectionArgument<&Group::Remove>
              },
              Size {}
        {
            for (EntityType Entity : std::views::reverse(Viewer.GetSmallestPool()))
            {
                PushOnConstruct(Entity);
            }
        }

        [[nodiscard]] constexpr Iterator Begin() const noexcept
        {
            return Viewer.GetIterator(EntitiesBegin());
        }

        [[nodiscard]] constexpr Iterator End() const noexcept
        {
            return Viewer.GetIterator(EntitiesEnd());
        }

        [[nodiscard]] constexpr CommonIterable Entities() const noexcept
        {
            return CommonIterable { EntitiesBegin(), EntitiesEnd() };
        }

        [[nodiscard]] constexpr CommonReverseIterable EntitiesReverse() const noexcept
        {
            return CommonReverseIterable { EntitiesReverseBegin(), EntitiesReverseEnd() };
        }

        [[nodiscard]] constexpr CommonIterator EntitiesBegin() const noexcept
        {
            return GetLeading().End() - Size;
        }

        [[nodiscard]] constexpr CommonIterator EntitiesEnd() const noexcept
        {
            return GetLeading().End();
        }

        [[nodiscard]] constexpr CommonReverseIterator EntitiesReverseBegin() const noexcept
        {
            return GetLeading().ReverseBegin();
        }

        [[nodiscard]] constexpr CommonReverseIterator EntitiesReverseEnd() const noexcept
        {
            return GetLeading().ReverseBegin() + Size;
        }

        template <typename CallableType> requires
            Types::Applicable<CallableType, std::iter_reference_t<Iterator>> ||
            Types::Applicable<CallableType, Types::RemoveTupleType<EntityType, std::iter_reference_t<Iterator>>>
        constexpr void Each(CallableType Callable) const
        {
            Viewer.Each(EntitiesBegin(), EntitiesEnd(), std::move(Callable));
        }

        template <Types::ContainedIn<typename OwnParameters::ElementType..., typename ViewParameters::ElementType...>... ElementTypes>
            requires Types::AllUnique<ElementTypes...> &&
            (sizeof...(ElementTypes)
                 ? (!OptimizableElement<ElementTypes, EntityType> || ...)
                 : (!OptimizableElement<typename OwnParameters::ElementType, EntityType> || ...) ||
                 (!OptimizableElement<typename ViewParameters::ElementType, EntityType> || ...))
        [[nodiscard]] constexpr decltype(auto) Get(const EntityType Entity) const
        {
            return Viewer.Get(Entity);
        }

        [[nodiscard]] constexpr auto operator[](const std::size_t Position) const
        {
            const EntityType Entity { EntitiesBegin()[Position] };

            if constexpr (
                (!OptimizableElement<typename OwnParameters::ElementType, EntityType> || ...) ||
                (!OptimizableElement<typename ViewParameters::ElementType, EntityType> || ...))
            {
                return std::tuple_cat(std::make_tuple(Entity), Get(Entity));
            }
            else
            {
                return Entity;
            }
        }

        [[nodiscard]] constexpr Iterator Find(const EntityType Entity) const noexcept
        {
            if (const auto Found { GetLeading().Find(Entity) }; Found >= EntitiesBegin())
            {
                return Viewer.GetIterator(Found);
            }

            return End();
        }

        [[nodiscard]] constexpr bool Contains(const EntityType Entity) const noexcept
        {
            return GetLeading().Contains(Entity) && GetLeading().GetIndex(Entity) < Size;
        }

        [[nodiscard]] constexpr std::size_t GetSize() const noexcept
        {
            return Size;
        }

        [[nodiscard]] constexpr bool Empty() const noexcept
        {
            return !Size;
        }

        template <Types::ContainedIn<typename OwnParameters::ElementType..., typename ViewParameters::ElementType...>... ElementTypes,
            typename CompareType,
            typename SortType = decltype(std::ranges::sort),
            typename ProjectionType = std::identity> requires
            Types::AllUnique<ElementTypes...> && (!sizeof...(ElementTypes) || (!OptimizableElement<ElementTypes, EntityType> || ...))
        constexpr void Sort(CompareType Compare, SortType Sort = SortType {}, ProjectionType Projection = ProjectionType {}) const
        {
            GetLeading().SortCount(
                Size,
                std::move(Compare),
                std::move(Sort),
                Viewer.template GetProjection<ElementTypes...>(std::move(Projection))
            );

            if constexpr (sizeof...(OwnParameters) > 1u)
            {
                SortAsLeading<OwnParameters...>();
            }
        }

        template <typename IteratorType, std::sentinel_for<IteratorType> SentinelType>
        constexpr ReverseIterator SortAs(IteratorType First, SentinelType Last) const
        {
            CommonReverseIterator SortedIterator { GetLeading().SortCountAs(Size, std::move(First), std::move(Last)) };

            if constexpr (sizeof...(OwnParameters) > 1u)
            {
                SortAsLeading<OwnParameters...>();
            }

            return Viewer.GetReverseIterator(SortedIterator);
        }

    private:
        using LeadingType = std::tuple_element_t<0u, std::tuple<OwnParameters...>>;

        template <std::same_as<LeadingType>, Types::ContainedIn<OwnParameters...>... Other>
            requires (sizeof...(Other) != 0u && Types::AllUnique<Other...> && (!std::same_as<Other, LeadingType> && ...))
        constexpr void SortAsLeading() const
        {
            for (std::size_t Position = Size; Position--;)
            {
                const EntityType Entity { GetLeading()[Position] };
                (Viewer.template GetPool<Other>().SwapElementsAt(Position, Viewer.template GetPool<Other>().GetIndex(Entity)), ...);
            }
        }

        constexpr void PushOnConstruct(const EntityType Entity)
        {
            if (Viewer.Groupable(Entity))
            {
                SwapElements(Size++, Entity);
            }
        }

        constexpr void PushOnDestroy(const EntityType Entity)
        {
            if (Viewer.GroupableWithoutLastFilter(Entity))
            {
                SwapElements(Size++, Entity);
            }
        }

        constexpr void Remove(const EntityType Entity)
        {
            if (Contains(Entity))
            {
                SwapElements(--Size, Entity);
            }
        }

        constexpr void SwapElements(const std::size_t Position, const EntityType Entity)
        {
            (Viewer.template GetPool<OwnParameters>().SwapElementsAt(
                Position,
                Viewer.template GetPool<OwnParameters>().GetIndex(Entity)
            ), ...);
        }

        [[nodiscard]] constexpr LeadingType& GetLeading() const noexcept
        {
            return Viewer.template GetPool<LeadingType>();
        }

        Internal::GroupViewer<OwnType<OwnParameters...>, ViewType<ViewParameters...>, ExcludeType<ExcludeParameters...>> Viewer;
        std::size_t Size;
    };


    template <Types::InstanceOf<Lifecycle>... ViewParameters, Types::InstanceOf<Lifecycle>... ExcludeParameters> requires
        (Types::InstanceOf<typename ViewParameters::ContainerType, Storage> && ...) &&
        (Types::InstanceOf<typename ExcludeParameters::ContainerType, Storage> && ...) &&
        Types::AllUnique<typename ViewParameters::ElementType..., typename ExcludeParameters::ElementType...> &&
        Types::AllSame<typename ViewParameters::BaseType..., typename ExcludeParameters::BaseType...> &&
        (sizeof...(ViewParameters) != 0u) && (sizeof...(ViewParameters) != 1u || sizeof...(ExcludeParameters) != 0u)
    class Group<OwnType<>, ViewType<ViewParameters...>, ExcludeType<ExcludeParameters...>>
    {
    public:
        using CommonType = CommonTypeOf<
            typename ViewParameters::BaseType...,
            typename ExcludeParameters::BaseType...
        >;
        using CommonAllocator = typename CommonType::AllocatorType;

        using CommonIterator = typename CommonType::Iterator;
        using CommonReverseIterator = typename CommonType::ReverseIterator;

        using CommonIterable = egg::Containers::IterableAdaptor<CommonIterator>;
        using CommonReverseIterable = egg::Containers::IterableAdaptor<CommonReverseIterator>;

        using EntityType = typename CommonType::EntityType;

        using Iterator = Internal::GroupIterator<CommonIterator, OwnType<>, ViewType<ViewParameters...>>;
        using ReverseIterator = Internal::GroupIterator<CommonReverseIterator, OwnType<>, ViewType<ViewParameters...>>;


        constexpr Group(const CommonAllocator& Allocator,
                        const std::tuple<ViewParameters&...> Pools, const std::tuple<ExcludeParameters&...> Filters)
            : Viewer {
                  Pools, Filters, *this,
                  Events::ConnectionArgument<&Group::PushOnConstruct>,
                  Events::ConnectionArgument<&Group::PushOnDestroy>,
                  Events::ConnectionArgument<&Group::Remove>
              },
              Container { Allocator }
        {
            for (EntityType Entity : Viewer.GetSmallestPool())
            {
                PushOnConstruct(Entity);
            }
        }

        [[nodiscard]] constexpr Iterator Begin() const noexcept
        {
            return Viewer.GetIterator(EntitiesBegin());
        }

        [[nodiscard]] constexpr Iterator End() const noexcept
        {
            return Viewer.GetIterator(EntitiesEnd());
        }

        [[nodiscard]] constexpr CommonIterable Entities() const noexcept
        {
            return CommonIterable { EntitiesBegin(), EntitiesEnd() };
        }

        [[nodiscard]] constexpr CommonReverseIterable EntitiesReverse() const noexcept
        {
            return CommonReverseIterable { EntitiesReverseBegin(), EntitiesReverseEnd() };
        }

        [[nodiscard]] constexpr CommonIterator EntitiesBegin() const noexcept
        {
            return Container.Begin();
        }

        [[nodiscard]] constexpr CommonIterator EntitiesEnd() const noexcept
        {
            return Container.End();
        }

        [[nodiscard]] constexpr CommonReverseIterator EntitiesReverseBegin() const noexcept
        {
            return Container.ReverseBegin();
        }

        [[nodiscard]] constexpr CommonReverseIterator EntitiesReverseEnd() const noexcept
        {
            return Container.ReverseEnd();
        }

        template <typename CallableType> requires
            Types::Applicable<CallableType, std::iter_reference_t<Iterator>> ||
            Types::Applicable<CallableType, Types::RemoveTupleType<EntityType, std::iter_reference_t<Iterator>>>
        constexpr void Each(CallableType Callable) const
        {
            Viewer.Each(EntitiesBegin(), EntitiesEnd(), std::move(Callable));
        }

        template <Types::ContainedIn<typename ViewParameters::ElementType...>... ElementTypes>
            requires Types::AllUnique<ElementTypes...> &&
            (sizeof...(ElementTypes)
                 ? (!OptimizableElement<ElementTypes, EntityType> || ...)
                 : (!OptimizableElement<typename ViewParameters::ElementType, EntityType> || ...))
        [[nodiscard]] constexpr decltype(auto) Get(const EntityType Entity) const
        {
            return Viewer.Get(Entity);
        }

        [[nodiscard]] constexpr auto operator[](const std::size_t Position) const
        {
            const EntityType Entity { EntitiesBegin()[Position] };

            if constexpr ((!OptimizableElement<typename ViewParameters::ElementType, EntityType> || ...))
            {
                return std::tuple_cat(std::make_tuple(Entity), Get(Entity));
            }
            else
            {
                return Entity;
            }
        }

        [[nodiscard]] constexpr Iterator Find(const EntityType Entity) const noexcept
        {
            return Viewer.GetIterator(Container.Find(Entity));
        }

        [[nodiscard]] constexpr bool Contains(const EntityType Entity) const noexcept
        {
            return Container.Contains(Entity);
        }

        [[nodiscard]] constexpr std::size_t GetSize() const noexcept
        {
            return Container.GetSize();
        }

        [[nodiscard]] constexpr bool Empty() const noexcept
        {
            return Container.Empty();
        }

        [[nodiscard]] constexpr std::size_t GetCapacity() const noexcept
        {
            return Container.GetCapacity();
        }

        template <Types::ContainedIn<typename ViewParameters::ElementType...>... ElementTypes,
            typename CompareType,
            typename SortType = decltype(std::ranges::sort),
            typename ProjectionType = std::identity> requires
            Types::AllUnique<ElementTypes...> && (!sizeof...(ElementTypes) || (!OptimizableElement<ElementTypes, EntityType> || ...))
        constexpr void Sort(CompareType Compare, SortType Sort = SortType {}, ProjectionType Projection = ProjectionType {}) const
        {
            Container.Sort(std::move(Compare), std::move(Sort), Viewer.template GetProjection<ElementTypes...>(std::move(Projection)));
        }

        template <typename IteratorType, std::sentinel_for<IteratorType> SentinelType>
        constexpr Iterator SortAs(IteratorType First, SentinelType Last) const
        {
            return Viewer.GetIterator(Container.SortAs(std::move(First), std::move(Last)));
        }

        constexpr void ShrinkToFit()
        {
            Container.ShrinkToFit();
        }

    private:
        constexpr void PushOnConstruct(const EntityType Entity)
        {
            if (Viewer.Groupable(Entity))
            {
                Container.Push(Entity);
            }
        }

        constexpr void PushOnDestroy(const EntityType Entity)
        {
            if (Viewer.GroupableWithoutLastFilter(Entity))
            {
                Container.Push(Entity);
            }
        }

        constexpr void Remove(const EntityType Entity)
        {
            Container.Remove(Entity);
        }

        Internal::GroupViewer<OwnType<>, ViewType<ViewParameters...>, ExcludeType<ExcludeParameters...>> Viewer;
        CommonType Container;
    };


    template <
        Types::InstanceOf<Lifecycle>... OwnParameters,
        Types::InstanceOf<Lifecycle>... ViewParameters
    > requires
        (Types::InstanceOf<typename OwnParameters::ContainerType, Storage> && ...) &&
        (Types::InstanceOf<typename ViewParameters::ContainerType, Storage> && ...) &&
        (sizeof...(OwnParameters) + sizeof...(ViewParameters) == 1u)
    class Group<OwnType<OwnParameters...>, ViewType<ViewParameters...>, ExcludeType<>>
    {
    public:
        using PoolType = std::tuple_element_t<
            0u,
            std::conditional_t<
                sizeof...(OwnParameters),
                std::tuple<OwnParameters...>,
                std::tuple<ViewParameters...>
            >
        >;
        using CommonType = typename PoolType::BaseType;
        using CommonAllocator = typename CommonType::AllocatorType;

        using CommonIterator = typename CommonType::Iterator;
        using CommonReverseIterator = typename CommonType::ReverseIterator;

        using CommonIterable = egg::Containers::IterableAdaptor<CommonIterator>;
        using CommonReverseIterable = egg::Containers::IterableAdaptor<CommonReverseIterator>;

        using EntityType = typename CommonType::EntityType;
        using ElementType = typename PoolType::ElementType;

        using Iterator = typename PoolType::EachIterator;
        using ReverseIterator = typename PoolType::EachReverseIterator;


        explicit constexpr Group(PoolType& Pool) : Pool { Pool }
        {
        }

        [[nodiscard]] constexpr Iterator Begin() const noexcept
        {
            return Pool.EachBegin();
        }

        [[nodiscard]] constexpr Iterator End() const noexcept
        {
            return Pool.EachEnd();
        }

        [[nodiscard]] constexpr CommonIterable Entities() const noexcept
        {
            return CommonIterable { EntitiesBegin(), EntitiesEnd() };
        }

        [[nodiscard]] constexpr CommonReverseIterable EntitiesReverse() const noexcept
        {
            return CommonReverseIterable { EntitiesReverseBegin(), EntitiesReverseEnd() };
        }

        [[nodiscard]] constexpr CommonIterator EntitiesBegin() const noexcept
        {
            return Pool.Begin();
        }

        [[nodiscard]] constexpr CommonIterator EntitiesEnd() const noexcept
        {
            return Pool.End();
        }

        [[nodiscard]] constexpr CommonReverseIterator EntitiesReverseBegin() const noexcept
        {
            return Pool.ReverseBegin();
        }

        [[nodiscard]] constexpr CommonReverseIterator EntitiesReverseEnd() const noexcept
        {
            return Pool.ReverseEnd();
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

        [[nodiscard]] constexpr ElementType& Get(const EntityType Entity) const requires (!OptimizableElement<ElementType, EntityType>)
        {
            return Pool.Get(Entity);
        }

        [[nodiscard]] constexpr auto operator[](const std::size_t Position) const
        {
            const EntityType Entity { EntitiesBegin()[Position] };

            if constexpr (!OptimizableElement<ElementType, EntityType>)
            {
                return std::tuple<EntityType, ElementType&> { Entity, Get(Entity) };
            }
            else
            {
                return Entity;
            }
        }

        [[nodiscard]] constexpr Iterator Find(const EntityType Entity) const noexcept
        {
            return GetIterator(Pool.Find(Entity));
        }

        [[nodiscard]] constexpr bool Contains(const EntityType Entity) const noexcept
        {
            return Pool.Contains(Entity);
        }

        [[nodiscard]] constexpr std::size_t GetSize() const noexcept
        {
            return Pool.GetSize();
        }

        [[nodiscard]] constexpr bool Empty() const noexcept
        {
            return Pool.Empty();
        }

        [[nodiscard]] constexpr std::size_t GetCapacity() const noexcept requires (sizeof...(OwnParameters) == 1u)
        {
            return Pool.GetCapacity();
        }

        template <std::same_as<ElementType>... ElementTypes,
            typename CompareType,
            typename SortType = decltype(std::ranges::sort),
            typename ProjectionType = std::identity> requires (sizeof...(ElementTypes) <= 1u)
        constexpr void Sort(CompareType Compare, SortType Sort = SortType {}, ProjectionType Projection = ProjectionType {}) const
            requires (sizeof...(OwnParameters) == 1u)
        {
            if constexpr (!sizeof...(ElementTypes))
            {
                Pool.Sort(std::move(Compare), std::move(Sort), std::move(Projection));
            }
            else
            {
                Pool.Sort(
                    std::move(Compare),
                    std::move(Sort),
                    [this, &Projection](const EntityType Entity) constexpr
                    {
                        return std::invoke(Projection, Get(Entity));
                    }
                );
            }
        }

        template <typename IteratorType, std::sentinel_for<IteratorType> SentinelType>
        constexpr Iterator SortAs(IteratorType First, SentinelType Last) const requires (sizeof...(OwnParameters) == 1u)
        {
            return GetIterator(Pool.SortAs(std::move(First), std::move(Last)));
        }

        constexpr void ShrinkToFit() requires (sizeof...(OwnParameters) == 1u)
        {
            Pool.ShrinkToFit();
        }

    private:
        [[nodiscard]] constexpr Iterator GetIterator(const CommonIterator EntitiesIterator) const noexcept
        {
            return Iterator { EntitiesIterator, Pool.ElementsEnd() - (EntitiesIterator.GetIndex() + 1u) };
        }

        PoolType& Pool;
    };
}

#endif // ENGINE_SOURCES_ECS_CONTAINERS_GROUP_FILE_GROUP_H
