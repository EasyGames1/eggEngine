#ifndef ENGINE_SOURCES_ECS_CONTAINERS_GROUP_INTERNAL_FILE_GROUP_ITERATOR_H
#define ENGINE_SOURCES_ECS_CONTAINERS_GROUP_INTERNAL_FILE_GROUP_ITERATOR_H

#include <Containers/PointerImitator.h>
#include <ECS/Ownership.h>
#include <ECS/Containers/LifecycleContainer.h>
#include <Types/Capabilities/Capabilities.h>

#include <cstddef>
#include <iterator>
#include <tuple>

namespace egg::ECS::Containers::Internal
{
    template <typename, Types::InstanceOf<OwnType>, Types::InstanceOf<ViewType>>
    class GroupIterator;


    template <typename IteratorParameter,
        Types::InstanceOf<Lifecycle>... OwnParameters,
        Types::InstanceOf<Lifecycle>... ViewParameters
    > requires
        (Types::InstanceOf<typename OwnParameters::ContainerType, Storage> && ...) &&
        (Types::InstanceOf<typename ViewParameters::ContainerType, Storage> && ...) &&
        Types::AllUnique<typename OwnParameters::ElementType..., typename ViewParameters::ElementType...> &&
        Types::AllSame<typename OwnParameters::BaseType..., typename ViewParameters::BaseType...>
    class GroupIterator<IteratorParameter, OwnType<OwnParameters...>, ViewType<ViewParameters...>>
    {
        template <Types::ContainedIn<OwnParameters...> PoolType>
            requires (!OptimizableElement<typename PoolType::ElementType, typename PoolType::EntityType>)
        [[nodiscard]] constexpr std::tuple<typename PoolType::ElementType&> GetElementAsTuple(PoolType& Pool) const noexcept
        {
            return std::forward_as_tuple(Pool.ElementsReverseBegin()[Iterator.GetIndex()]);
        }

        template <Types::ContainedIn<ViewParameters...> PoolType>
            requires (!OptimizableElement<typename PoolType::ElementType, typename PoolType::EntityType>)
        [[nodiscard]] constexpr std::tuple<typename PoolType::ElementType&> GetElementAsTuple(PoolType& Pool) const noexcept
        {
            return std::forward_as_tuple(Pool.Get(*Iterator));
        }

        template <Types::ContainedIn<OwnParameters..., ViewParameters...> PoolType>
            requires OptimizableElement<typename PoolType::ElementType, typename PoolType::EntityType>
        [[nodiscard]] static constexpr std::tuple<> GetElementAsTuple(PoolType&) noexcept
        {
            return std::make_tuple();
        }

        template <typename ElementType>
        using StorablePredicate = std::bool_constant<!OptimizableElement<ElementType, std::iter_value_t<IteratorParameter>>>;

    public:
        using IteratorType = IteratorParameter;
        using value_type = Types::CombineTuples<
            std::tuple,
            std::tuple<std::iter_value_t<IteratorType>>,
            Types::InstantiateTuple<
                std::add_lvalue_reference_t,
                Types::FilterTuple<
                    StorablePredicate,
                    std::tuple<typename OwnParameters::ElementType..., typename ViewParameters::ElementType...>
                >
            >
        >;
        using pointer = egg::Containers::PointerImitator<value_type>;
        using reference = value_type;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::input_iterator_tag;
        using iterator_concept = std::forward_iterator_tag;


        constexpr GroupIterator() : Iterator {}, Pools {}
        {
        }

        constexpr GroupIterator(IteratorType From, const std::tuple<OwnParameters*..., ViewParameters*...>& Pools)
            : Iterator { From }, Pools { Pools }
        {
        }

        constexpr GroupIterator& operator++() noexcept
        {
            ++Iterator;
            return *this;
        }

        constexpr GroupIterator operator++(int) noexcept
        {
            GroupIterator Original { *this };
            ++*this;
            return Original;
        }

        [[nodiscard]] constexpr reference operator*() const noexcept
        {
            return std::tuple_cat(
                std::make_tuple(*Iterator),
                GetElementAsTuple(*std::get<OwnParameters*>(Pools))...,
                GetElementAsTuple(*std::get<ViewParameters*>(Pools))...
            );
        }

        [[nodiscard]] constexpr pointer operator->() const noexcept
        {
            return pointer { operator*() };
        }

        [[nodiscard]] constexpr IteratorType Base() const noexcept
        {
            return Iterator;
        }

        [[nodiscard]] constexpr bool operator==(const GroupIterator& Other) const noexcept
        {
            return Iterator == Other.Iterator;
        }

        [[nodiscard]] constexpr bool operator!=(const GroupIterator& Other) const noexcept
        {
            return !(*this == Other);
        }

    private:
        IteratorType Iterator;
        std::tuple<OwnParameters*..., ViewParameters*...> Pools;
    };
}

#endif // ENGINE_SOURCES_ECS_CONTAINERS_GROUP_INTERNAL_FILE_GROUP_ITERATOR_H
