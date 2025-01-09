#ifndef ENGINE_SOURCES_ECS_CONTAINERS_POOL_GROUP_FILE_GROUP_H
#define ENGINE_SOURCES_ECS_CONTAINERS_POOL_GROUP_FILE_GROUP_H

#include "./Internal/PoolGroupViewer.h"

#include <ECS/Ownership.h>
#include <ECS/Containers/Lifecycle.h>
#include <Types/Capabilities/Capabilities.h>

#include <algorithm>
#include <iterator>
#include <ranges>
#include <tuple>

namespace egg::ECS::Containers
{
    template <Types::InstanceOf<OwnType>, Types::InstanceOf<ViewType>, Types::InstanceOf<ExcludeType>>
    class PoolGroup;


    template <
        Types::InstanceOf<Lifecycle>... OwnParameters,
        Types::InstanceOf<Lifecycle>... ViewParameters,
        Types::InstanceOf<Lifecycle>... ExcludeParameters
    > requires
        Types::NoneOf<std::is_const, OwnParameters..., ViewParameters..., ExcludeParameters...> &&
        (Types::InstanceOf<typename OwnParameters::ContainerType, Storage> && ...) &&
        (Types::InstanceOf<typename ViewParameters::ContainerType, Storage> && ...) &&
        (Types::InstanceOf<typename ExcludeParameters::ContainerType, Storage> && ...) &&
        Types::AllUnique<typename OwnParameters::ElementType...,
                         typename ViewParameters::ElementType...,
                         typename ExcludeParameters::ElementType...> &&
        Types::AllSame<typename OwnParameters::BaseType...,
                       typename ViewParameters::BaseType...,
                       typename ExcludeParameters::BaseType...> &&
        (sizeof...(OwnParameters) + sizeof...(ViewParameters) != 0u)
    class PoolGroup<OwnType<OwnParameters...>, ViewType<ViewParameters...>, ExcludeType<ExcludeParameters...>>
    {
        using ViewerType = Internal::PoolGroupViewer<
            OwnType<OwnParameters...>,
            ViewType<ViewParameters...>,
            ExcludeType<ExcludeParameters...>
        >;

        using CommonType = typename ViewerType::CommonType;

        using LeadingType = std::tuple_element_t<0u, std::tuple<OwnParameters...>>;

    public:
        using PoolsType = typename ViewerType::PoolsType;
        using FiltersType = typename ViewerType::FiltersType;

        using AllocatorType = typename CommonType::AllocatorType;
        using EntityType = typename CommonType::EntityType;

        using Iterator = typename CommonType::Iterator;
        using ReverseIterator = typename CommonType::ReverseIterator;


        constexpr PoolGroup(const PoolsType Pools, const FiltersType Filters)
            : Viewer {
                  Pools, Filters, *this,
                  Events::ConnectionArgument<&PoolGroup::PushOnConstruct>,
                  Events::ConnectionArgument<&PoolGroup::PushOnDestroy>,
                  Events::ConnectionArgument<&PoolGroup::Remove>
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
            return GetLeading().End() - Size;
        }

        [[nodiscard]] constexpr Iterator End() const noexcept
        {
            return GetLeading().End();
        }

        [[nodiscard]] constexpr ReverseIterator ReverseBegin() const noexcept
        {
            return GetLeading().ReverseBegin();
        }

        [[nodiscard]] constexpr ReverseIterator ReverseEnd() const noexcept
        {
            return GetLeading().ReverseBegin() + Size;
        }

        template <typename... ElementTypes> requires
            (Types::ContainedIn<
                std::remove_const_t<ElementTypes>,
                typename OwnParameters::ElementType..., typename ViewParameters::ElementType...
            > && ...) &&
            Types::AllUnique<std::remove_const_t<ElementTypes>...> &&
            (!OptimizableElement<ElementTypes, EntityType> || ...)
        [[nodiscard]] constexpr decltype(auto) Get(const EntityType Entity) const
        {
            return Viewer.template GetElements<ElementTypes...>(Entity);
        }

        [[nodiscard]] constexpr Iterator Find(const EntityType Entity) const noexcept
        {
            if (const auto Found { GetLeading().Find(Entity) }; Found >= Begin())
            {
                return Found;
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

        [[nodiscard]] constexpr const PoolsType& GetPools() const noexcept
        {
            return Viewer.GetPools();
        }

        [[nodiscard]] constexpr const FiltersType& GetFilters() const noexcept
        {
            return Viewer.GetFilters();
        }

        template <typename... ElementTypes,
                  typename CompareType,
                  typename SortType = decltype(std::ranges::sort),
                  typename ProjectionType = std::identity> requires
            (Types::ContainedIn<
                std::remove_const_t<ElementTypes>,
                typename OwnParameters::ElementType..., typename ViewParameters::ElementType...
            > && ...) &&
            Types::AllUnique<std::remove_const_t<ElementTypes>...>
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
                SortCountAsLeading<OwnParameters...>(Size);
            }
        }

        template <typename IteratorType, std::sentinel_for<IteratorType> SentinelType>
        constexpr Iterator SortAs(IteratorType First, SentinelType Last) const
        {
            Iterator SortedIterator { GetLeading().SortCountAs(Size, std::move(First), std::move(Last)) };

            if constexpr (sizeof...(OwnParameters) > 1u)
            {
                SortCountAsLeading<OwnParameters...>(SortedIterator.GetIndex() + 1u);
            }

            return SortedIterator;
        }

    private:
        template <std::same_as<LeadingType>, Types::ContainedIn<OwnParameters...>... Other>
            requires (sizeof...(Other) != 0u && Types::AllUnique<Other...> && (!std::same_as<Other, LeadingType> && ...))
        constexpr void SortCountAsLeading(const std::size_t Count) const
        {
            for (std::size_t Position = Count; Position--;)
            {
                const EntityType Entity { GetLeading()[Position] };
                (Viewer.template GetPool<Other>().SwapElementsAt(Position, Viewer.template GetPool<Other>().GetIndex(Entity)), ...);
            }
        }

        [[nodiscard]] constexpr std::tuple_element_t<0u, std::tuple<OwnParameters...>>& GetLeading() const noexcept
        {
            return Viewer.template GetPool<std::tuple_element_t<0u, std::tuple<OwnParameters...>>>();
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

        ViewerType Viewer;
        std::size_t Size;
    };


    template <Types::InstanceOf<Lifecycle>... ViewParameters, Types::InstanceOf<Lifecycle>... ExcludeParameters> requires
        Types::NoneOf<std::is_const, ViewParameters..., ExcludeParameters...> &&
        (Types::InstanceOf<typename ViewParameters::ContainerType, Storage> && ...) &&
        (Types::InstanceOf<typename ExcludeParameters::ContainerType, Storage> && ...) &&
        Types::AllUnique<typename ViewParameters::ElementType..., typename ExcludeParameters::ElementType...> &&
        Types::AllSame<typename ViewParameters::BaseType..., typename ExcludeParameters::BaseType...> &&
        (sizeof...(ViewParameters) != 0u)
    class PoolGroup<OwnType<>, ViewType<ViewParameters...>, ExcludeType<ExcludeParameters...>>
    {
        using ViewerType = Internal::PoolGroupViewer<OwnType<>, ViewType<ViewParameters...>, ExcludeType<ExcludeParameters...>>;

        using CommonType = typename ViewerType::CommonType;

    public:
        using PoolsType = typename ViewerType::PoolsType;
        using FiltersType = typename ViewerType::FiltersType;

        using AllocatorType = typename CommonType::AllocatorType;
        using EntityType = typename CommonType::EntityType;

        using Iterator = typename CommonType::Iterator;
        using ReverseIterator = typename CommonType::ReverseIterator;


        constexpr PoolGroup(const std::tuple<ViewParameters&...> Pools, const std::tuple<ExcludeParameters&...> Filters,
                            const AllocatorType& Allocator = {})
            : Viewer {
                  Pools, Filters, *this,
                  Events::ConnectionArgument<&PoolGroup::PushOnConstruct>,
                  Events::ConnectionArgument<&PoolGroup::PushOnDestroy>,
                  Events::ConnectionArgument<&PoolGroup::Remove>
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
            return Container.Begin();
        }

        [[nodiscard]] constexpr Iterator End() const noexcept
        {
            return Container.End();
        }

        [[nodiscard]] constexpr ReverseIterator ReverseBegin() const noexcept
        {
            return Container.ReverseBegin();
        }

        [[nodiscard]] constexpr ReverseIterator ReverseEnd() const noexcept
        {
            return Container.ReverseEnd();
        }

        template <typename... ElementTypes> requires
            (Types::ContainedIn<std::remove_const_t<ElementTypes>, typename ViewParameters::ElementType...> && ...) &&
            Types::AllUnique<std::remove_const_t<ElementTypes>...> &&
            (!OptimizableElement<ElementTypes, EntityType> || ...)
        [[nodiscard]] constexpr decltype(auto) Get(const EntityType Entity) const
        {
            return Viewer.template GetElements<ElementTypes...>(Entity);
        }

        [[nodiscard]] constexpr Iterator Find(const EntityType Entity) const noexcept
        {
            return Container.Find(Entity);
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

        [[nodiscard]] constexpr const PoolsType& GetPools() const noexcept
        {
            return Viewer.GetPools();
        }

        [[nodiscard]] constexpr const FiltersType& GetFilters() const noexcept
        {
            return Viewer.GetFilters();
        }

        template <typename... ElementTypes,
                  typename CompareType,
                  typename SortType = decltype(std::ranges::sort),
                  typename ProjectionType = std::identity> requires
            (Types::ContainedIn<std::remove_const_t<ElementTypes>, typename ViewParameters::ElementType...> && ...) &&
            Types::AllUnique<std::remove_const_t<ElementTypes>...>
        constexpr void Sort(CompareType Compare, SortType Sort = SortType {}, ProjectionType Projection = ProjectionType {}) const
        {
            Container.Sort(std::move(Compare), std::move(Sort), Viewer.template GetProjection<ElementTypes...>(std::move(Projection)));
        }

        template <typename IteratorType, std::sentinel_for<IteratorType> SentinelType>
        constexpr Iterator SortAs(IteratorType First, SentinelType Last) const
        {
            return Container.SortAs(std::move(First), std::move(Last));
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

        ViewerType Viewer;
        CommonType Container;
    };
}

#endif // ENGINE_SOURCES_ECS_CONTAINERS_POOL_GROUP_FILE_GROUP_H
