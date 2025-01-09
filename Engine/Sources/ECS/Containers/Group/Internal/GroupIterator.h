#ifndef ENGINE_SOURCES_ECS_CONTAINERS_GROUP_INTERNAL_FILE_GROUP_ITERATOR_H
#define ENGINE_SOURCES_ECS_CONTAINERS_GROUP_INTERNAL_FILE_GROUP_ITERATOR_H

#include "./PoolTraits.h"

#include <Containers/PointerImitator.h>
#include <ECS/Ownership.h>
#include <ECS/Containers/Lifecycle.h>
#include <ECS/Containers/Storage/Storage.h>
#include <Types/Capabilities/Capabilities.h>

#include <cstddef>
#include <iterator>
#include <memory>
#include <tuple>

namespace egg::ECS::Containers::Internal
{
    template <typename IteratorParameter, Types::InstanceOf<OwnType>, Types::InstanceOf<ViewType>,
        Types::ValidAllocator<std::iter_value_t<IteratorParameter>> = std::allocator<std::iter_value_t<IteratorParameter>>>
    class GroupIterator;


    template <typename IteratorParameter,
              typename... OwnParameters,
              typename... ViewParameters,
              Types::ValidAllocator<std::iter_value_t<IteratorParameter>> AllocatorParameter> requires
        Types::AllTupleUnique<Types::InstantiateTuple<
            std::remove_const_t,
            std::tuple<OwnParameters..., ViewParameters...>
        >>
    class GroupIterator<IteratorParameter, OwnType<OwnParameters...>, ViewType<ViewParameters...>, AllocatorParameter>
    {
        using EntityType = std::iter_value_t<IteratorParameter>;

        using TraitsType = PoolTraits<EntityType, AllocatorParameter>;

        template <typename ElementType>
        using PoolForElementType = typename TraitsType::template LifecycleStorageFor<ElementType>;

        using PoolsTuple = std::tuple<PoolForElementType<OwnParameters>*..., PoolForElementType<ViewParameters>*...>;

    public:
        template <typename ElementType>
        using PoolFor = PoolForElementType<ElementType>;

        using IteratorType = IteratorParameter;

        using value_type = typename TraitsType::template StorableTuple<OwnParameters..., ViewParameters...>;
        using pointer = egg::Containers::PointerImitator<value_type>;
        using reference = value_type;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::input_iterator_tag;
        using iterator_concept = std::forward_iterator_tag;


        constexpr GroupIterator() : Iterator {}, Pools {}
        {
        }

        constexpr GroupIterator(IteratorType From, const std::tuple<PoolFor<OwnParameters>&..., PoolFor<ViewParameters>&...>& Pools)
            : Iterator { From },
              Pools {
                  std::apply([](auto&... GroupPools) constexpr noexcept
                  {
                      return PoolsTuple { &GroupPools... };
                  }, Pools)
              }
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
                GetElementAsTuple<OwnParameters>()...,
                GetElementAsTuple<ViewParameters>()...
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
        template <Types::ContainedIn<OwnParameters..., ViewParameters...> ElementType>
        [[nodiscard]] constexpr Types::ConstnessAs<ElementType, PoolFor<ElementType>>& GetPoolFor() const noexcept
        {
            return *std::get<PoolFor<ElementType>*>(Pools);
        }

        template <Types::ContainedIn<OwnParameters...> ElementType>
            requires (!OptimizableElement<ElementType, EntityType>)
        [[nodiscard]] constexpr std::tuple<ElementType&> GetElementAsTuple() const noexcept
        {
            return std::forward_as_tuple(
                GetPoolFor<ElementType>().ElementsReverseBegin()[Iterator.GetIndex()]
            );
        }

        template <Types::ContainedIn<ViewParameters...> ElementType>
            requires (!OptimizableElement<ElementType, EntityType>)
        [[nodiscard]] constexpr std::tuple<ElementType&> GetElementAsTuple() const noexcept
        {
            return std::forward_as_tuple(GetPoolFor<ElementType>().Get(*Iterator));
        }

        template <Types::ContainedIn<OwnParameters..., ViewParameters...> ElementType>
            requires OptimizableElement<ElementType, EntityType>
        [[nodiscard]] static constexpr std::tuple<> GetElementAsTuple() noexcept
        {
            return std::make_tuple();
        }

        IteratorType Iterator;
        PoolsTuple Pools;
    };
}

#endif // ENGINE_SOURCES_ECS_CONTAINERS_GROUP_INTERNAL_FILE_GROUP_ITERATOR_H
