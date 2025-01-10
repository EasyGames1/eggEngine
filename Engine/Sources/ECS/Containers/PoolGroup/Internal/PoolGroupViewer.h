#ifndef ENGINE_SOURCES_ECS_CONTAINERS_GROUP_INTERNAL_FILE_GROUP_STORAGE_H
#define ENGINE_SOURCES_ECS_CONTAINERS_GROUP_INTERNAL_FILE_GROUP_STORAGE_H

#include <ECS/Ownership.h>
#include <ECS/Containers/Container.h>
#include <ECS/Containers/Lifecycle.h>
#include <ECS/Containers/Storage/Storage.h>
#include <Events/ConnectionArgument.h>
#include <Types/Capabilities/Capabilities.h>

#include <array>
#include <ranges>
#include <tuple>
#include <type_traits>

namespace egg::ECS::Containers::Internal
{
    template <Types::InstanceOf<OwnType>, Types::InstanceOf<ViewType>, Types::InstanceOf<ExcludeType>>
    class PoolGroupViewer;


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
    class PoolGroupViewer<OwnType<OwnParameters...>, ViewType<ViewParameters...>, ExcludeType<ExcludeParameters...>>
    {
        using PoolBaseType = CommonTypeOf<
            typename OwnParameters::BaseType...,
            typename ViewParameters::BaseType...,
            typename ExcludeParameters::BaseType...
        >;

        using EntityParameter = typename PoolBaseType::EntityType;

        template <typename ElementType>
        using StorablePredicate = std::bool_constant<!OptimizableElement<ElementType, EntityParameter>>;

    public:
        using PoolsType = std::tuple<OwnParameters&..., ViewParameters&...>;
        using FiltersType = std::tuple<ExcludeParameters&...>;

        using CommonType = PoolBaseType;
        using EntityType = EntityParameter;

        template <typename... ElementTypes>
        using StorableTupleOf = Types::FilterTuple<StorablePredicate, std::tuple<ElementTypes...>>;


        template <auto PushOnConstruct, auto PushOnDestroy, auto Remove, typename GroupType>
        constexpr PoolGroupViewer(const PoolsType& Pools,
                                  const FiltersType& Filters,
                                  GroupType& Group,
                                  Events::ConnectionArgumentType<PushOnConstruct>,
                                  Events::ConnectionArgumentType<PushOnDestroy>,
                                  Events::ConnectionArgumentType<Remove>) noexcept
            : Pools { Pools }, Filters { Filters }
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
                std::apply([Entity](auto&... GroupFilters) constexpr noexcept
                {
                    return (!GroupFilters.Contains(Entity) && ...);
                }, Filters);
        }

        [[nodiscard]] constexpr bool GroupableWithoutLastFilter(const EntityType Entity) const noexcept
        {
            return PoolsContain(Entity) &&
                std::apply([Entity](auto&... GroupFilters) constexpr noexcept
                {
                    return (0u + ... + GroupFilters.Contains(Entity)) == 1u;
                }, Filters);
        }

        template <typename PoolType> requires
            Types::ContainedIn<
                std::remove_const_t<PoolType>,
                OwnParameters..., ViewParameters..., ExcludeParameters...
            >
        [[nodiscard]] constexpr PoolType& GetPool() const noexcept
        {
            using Type = std::add_lvalue_reference_t<std::remove_const_t<PoolType>>;

            if constexpr (Types::ContainedInTuple<Type, PoolsType>)
            {
                return std::get<Type>(Pools);
            }
            else
            {
                return std::get<Type>(Filters);
            }
        }

        [[nodiscard]] constexpr const PoolsType& GetPools() const noexcept
        {
            return Pools;
        }

        [[nodiscard]] constexpr const FiltersType& GetFilters() const noexcept
        {
            return Filters;
        }

        [[nodiscard]] constexpr CommonType& GetSmallestPool() const noexcept
        {
            return *std::ranges::min(
                std::apply([](auto&... GroupPools) constexpr noexcept
                {
                    return std::array<CommonType*, sizeof...(OwnParameters) + sizeof...(ViewParameters)> { &GroupPools... };
                }, Pools),
                {},
                &CommonType::GetSize
            );
        }

        template <typename... ElementTypes> requires
            (Types::ContainedIn<
                std::remove_const_t<ElementTypes>,
                typename OwnParameters::ElementType..., typename ViewParameters::ElementType...
            > && ...) &&
            Types::AllUnique<std::remove_const_t<ElementTypes>...> &&
            (!OptimizableElement<ElementTypes, EntityType> || ...)
        [[nodiscard]] constexpr decltype(auto) GetElements(const EntityType Entity) const
        {
            if constexpr (!sizeof...(ElementTypes))
            {
                return GetElements<typename OwnParameters::ElementType..., typename ViewParameters::ElementType...>();
            }
            else if constexpr (Types::TupleSize<StorableTupleOf<ElementTypes...>> == 1u)
            {
                return GetElement<Types::CommonTypeOfTuple<StorableTupleOf<ElementTypes...>>>();
            }
            else
            {
                return std::tuple_cat(GetElementAsTuple<ElementTypes>(Entity)...);
            }
        }

        template <typename... ElementTypes, typename ProjectionType> requires
            (Types::ContainedIn<
                std::remove_const_t<ElementTypes>,
                typename OwnParameters::ElementType..., typename ViewParameters::ElementType...
            > && ...) &&
            Types::AllUnique<std::remove_const_t<ElementTypes>...>
        [[nodiscard]] constexpr auto GetProjection(ProjectionType Projection) const noexcept
        {
            if constexpr ((OptimizableElement<ElementTypes, EntityType> && ...))
            {
                return Projection;
            }
            else
            {
                return [this, Projection = std::move(Projection)](const EntityType Entity) constexpr
                {
                    return std::invoke(Projection, GetElements<ElementTypes...>(Entity));
                };
            }
        }

    private:
        template <typename ElementType> requires
            Types::ContainedIn<
                std::remove_const_t<ElementType>,
                typename OwnParameters::ElementType..., typename ViewParameters::ElementType...
            > && (!OptimizableElement<ElementType, EntityType>)
        [[nodiscard]] constexpr ElementType& GetElement(const EntityType Entity) const noexcept
        {
            return GetPool<Types::ConstnessAs<
                ElementType,
                std::tuple_element_t<
                    Types::TypeIndexIn<
                        std::remove_const_t<ElementType>,
                        typename OwnParameters::ElementType..., typename ViewParameters::ElementType...
                    >,
                    std::tuple<OwnParameters..., ViewParameters...>
                >
            >>().Get(Entity);
        }

        template <typename ElementType> requires
            Types::ContainedIn<
                std::remove_const_t<ElementType>,
                typename OwnParameters::ElementType..., typename ViewParameters::ElementType...
            > && (!OptimizableElement<ElementType, EntityType>)
        [[nodiscard]] constexpr std::tuple<ElementType&>
        GetElementAsTuple(const EntityType Entity) const noexcept
        {
            return std::forward_as_tuple(GetElement<ElementType>(Entity));
        }

        template <typename ElementType> requires
            Types::ContainedIn<
                std::remove_const_t<ElementType>,
                typename OwnParameters::ElementType..., typename ViewParameters::ElementType...
            > && OptimizableElement<ElementType, EntityType>
        [[nodiscard]] static constexpr std::tuple<> GetElementAsTuple(EntityType) noexcept
        {
            return std::make_tuple();
        }

        [[nodiscard]] constexpr bool PoolsContain(const EntityType Entity) const noexcept
        {
            return std::apply([Entity](auto&... GroupPools) constexpr noexcept { return (GroupPools.Contains(Entity) && ...); }, Pools);
        }

        PoolsType Pools;
        FiltersType Filters;
    };
}

#endif // ENGINE_SOURCES_ECS_CONTAINERS_GROUP_INTERNAL_FILE_GROUP_STORAGE_H
