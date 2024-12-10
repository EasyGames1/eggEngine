#ifndef ENGINE_SOURCES_ECS_CONTAINERS_GROUP_INTERNAL_FILE_GROUP_STORAGE_H
#define ENGINE_SOURCES_ECS_CONTAINERS_GROUP_INTERNAL_FILE_GROUP_STORAGE_H

#include "./GroupIterator.h"

#include <ECS/Ownership.h>
#include <ECS/Containers/LifecycleContainer.h>
#include <ECS/Containers/Storage/Storage.h>
#include <Events/ConnectionArgument.h>
#include <Types/Capabilities/Capabilities.h>

#include <array>
#include <functional>
#include <ranges>
#include <tuple>
#include <type_traits>

namespace egg::ECS::Containers::Internal
{
    template <Types::InstanceOf<OwnType>, Types::InstanceOf<ViewType>, Types::InstanceOf<ExcludeType>>
    class GroupViewer;

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
        Types::AllSame<typename OwnParameters::BaseType..., typename ViewParameters::BaseType..., typename ExcludeParameters::BaseType...>
    class GroupViewer<OwnType<OwnParameters...>, ViewType<ViewParameters...>, ExcludeType<ExcludeParameters...>>
    {
    public:
        using PoolsType = std::tuple<OwnParameters*..., ViewParameters*...>;
        using FiltersType = std::tuple<ExcludeParameters*...>;

        using CommonType = CommonTypeOf<
            typename OwnParameters::BaseType...,
            typename ViewParameters::BaseType...,
            typename ExcludeParameters::BaseType...
        >;

        using CommonIterator = typename CommonType::Iterator;
        using CommonReverseIterator = typename CommonType::ReverseIterator;

        using EntityType = typename CommonType::EntityType;

        using Iterator = GroupIterator<CommonIterator, OwnType<OwnParameters...>, ViewType<ViewParameters...>>;
        using ReverseIterator = GroupIterator<CommonReverseIterator, OwnType<OwnParameters...>, ViewType<ViewParameters...>>;


        template <auto PushOnConstruct, auto PushOnDestroy, auto Remove, typename GroupType>
        constexpr GroupViewer(const std::tuple<OwnParameters&..., ViewParameters&...>& Pools,
                              const std::tuple<ExcludeParameters&...>& Filters,
                              GroupType& Group,
                              Events::ConnectionArgumentType<PushOnConstruct>,
                              Events::ConnectionArgumentType<PushOnDestroy>,
                              Events::ConnectionArgumentType<Remove>) noexcept
            : Pools {
                  std::apply([](auto&... GroupPools) constexpr noexcept
                  {
                      return PoolsType { &GroupPools... };
                  }, Pools)
              },
              Filters {
                  std::apply([](auto&... GroupFilters) constexpr noexcept
                  {
                      return FiltersType { &GroupFilters... };
                  }, Filters)
              }
        {
            std::apply([&Group](auto&... GroupPools) constexpr
            {
                ((GroupPools.OnConstruct().template Connect<PushOnConstruct>(Group),
                    GroupPools.OnDestroy().template Connect<Remove>(Group)), ...);
            }, Pools);

            std::apply([&Group](auto&... GroupFilters) constexpr
            {
                ((GroupFilters.OnConstruct().template Connect<Remove>(Group),
                    GroupFilters.OnDestroy().template Connect<PushOnDestroy>(Group)), ...);
            }, Filters);
        }

        [[nodiscard]] constexpr bool Groupable(const EntityType Entity) const noexcept
        {
            return PoolsContain(Entity) &&
                std::apply([Entity](auto*... GroupFilters) constexpr noexcept
                {
                    return (!GroupFilters->Contains(Entity) && ...);
                }, Filters);
        }

        [[nodiscard]] constexpr bool GroupableWithoutLastFilter(const EntityType Entity) const noexcept
        {
            return PoolsContain(Entity) &&
                std::apply([Entity](auto*... GroupFilters) constexpr noexcept
                {
                    return (0u + ... + GroupFilters->Contains(Entity)) == 1u;
                }, Filters);
        }

        template <Types::ContainedIn<OwnParameters..., ViewParameters...> PoolType>
        [[nodiscard]] constexpr PoolType& GetPool() const noexcept
        {
            return *std::get<PoolType*>(Pools);
        }

        template <typename CallableType> requires
            Types::Applicable<CallableType, std::iter_reference_t<Iterator>> ||
            Types::Applicable<CallableType, Types::RemoveTupleType<EntityType, std::iter_reference_t<Iterator>>>
        constexpr void Each(const CommonIterator Begin, const CommonIterator End, CallableType Callable) const
        {
            for (Iterator First = GetIterator(Begin), Last = GetIterator(End); First != Last; ++First)
            {
                if constexpr (Types::Applicable<CallableType, std::iter_reference_t<Iterator>>)
                {
                    std::apply(Callable, *First);
                }
                else
                {
                    std::apply([&Callable]<typename... ElementTypes>(EntityType, ElementTypes&&... Elements) constexpr
                    {
                        Callable(std::forward<ElementTypes>(Elements)...);
                    }, *First);
                }
            }
        }

        [[nodiscard]] constexpr Iterator GetIterator(const CommonIterator EntitiesIterator) const noexcept
        {
            return Iterator { EntitiesIterator, Pools };
        }

        [[nodiscard]] constexpr ReverseIterator GetReverseIterator(const CommonReverseIterator EntitiesIterator) const noexcept
        {
            return ReverseIterator { EntitiesIterator, Pools };
        }

        template <Types::ContainedIn<typename OwnParameters::ElementType..., typename ViewParameters::ElementType...>... ElementTypes>
            requires Types::AllUnique<ElementTypes...> &&
            (sizeof...(ElementTypes)
                 ? (!OptimizableElement<ElementTypes, EntityType> || ...)
                 : (!OptimizableElement<typename OwnParameters::ElementType, EntityType> || ...) ||
                 (!OptimizableElement<typename ViewParameters::ElementType, EntityType> || ...))
        [[nodiscard]] constexpr decltype(auto) Get(const EntityType Entity) const
        {
            if constexpr (!sizeof...(ElementTypes))
            {
                return std::tuple_cat(
                    GetElementAsTuple<typename OwnParameters::ElementType>(Entity)...,
                    GetElementAsTuple<typename ViewParameters::ElementType>(Entity)...
                );
            }
            else if constexpr (sizeof...(ElementTypes) == 1u)
            {
                return (GetElement<ElementTypes>(), ...);
            }
            else
            {
                return std::tuple_cat(GetElementAsTuple<ElementTypes>()...);
            }
        }

        template <Types::ContainedIn<typename OwnParameters::ElementType..., typename ViewParameters::ElementType...>... ElementTypes,
            typename ProjectionType> requires
            Types::AllUnique<ElementTypes...> && (!sizeof...(ElementTypes) || (!OptimizableElement<ElementTypes, EntityType> || ...))
        [[nodiscard]] constexpr auto GetProjection(ProjectionType Projection) const noexcept
        {
            if constexpr (!sizeof...(ElementTypes))
            {
                return Projection;
            }
            else
            {
                return [this, &Projection](const EntityType Entity) constexpr
                {
                    return std::invoke(Projection, Get<ElementTypes...>(Entity));
                };
            }
        }

        [[nodiscard]] constexpr CommonType& GetSmallestPool() const noexcept
        {
            return *std::ranges::min(
                std::apply([](auto*... GroupPools) constexpr noexcept
                {
                    return std::array<CommonType*, sizeof...(OwnParameters) + sizeof...(ViewParameters)> { GroupPools... };
                }, Pools),
                {},
                &CommonType::GetSize
            );
        }

    private:
        template <Types::ContainedIn<typename OwnParameters::ElementType..., typename ViewParameters::ElementType...> ElementType>
            requires (!OptimizableElement<ElementType, EntityType>)
        [[nodiscard]] constexpr ElementType& GetElement(const EntityType Entity) const noexcept
        {
            return GetPool<std::remove_pointer_t<std::tuple_element_t<
                Types::TypeIndexIn<
                    ElementType,
                    std::tuple<typename OwnParameters::ElementType..., typename ViewParameters::ElementType...>
                >,
                PoolsType
            >>>().Get(Entity);
        }

        template <Types::ContainedIn<typename OwnParameters::ElementType..., typename ViewParameters::ElementType...> ElementType>
        [[nodiscard]] constexpr std::tuple<ElementType&>
        GetElementAsTuple(const EntityType Entity) const noexcept
        {
            return std::forward_as_tuple(GetElement<ElementType>(Entity));
        }

        template <Types::ContainedIn<OwnParameters..., ViewParameters...> PoolType>
            requires OptimizableElement<typename PoolType::ElementType, EntityType>
        [[nodiscard]] static constexpr std::tuple<> GetElementAsTuple(EntityType) noexcept
        {
            return std::make_tuple();
        }

        [[nodiscard]] constexpr bool PoolsContain(const EntityType Entity) const noexcept
        {
            return std::apply([Entity](auto*... GroupPools) constexpr noexcept { return (GroupPools->Contains(Entity) && ...); }, Pools);
        }

        PoolsType Pools;
        FiltersType Filters;
    };
}

#endif // ENGINE_SOURCES_ECS_CONTAINERS_GROUP_INTERNAL_FILE_GROUP_STORAGE_H
