#ifndef ENGINE_SOURCES_CONTAINERS_DENSE_MAP_INTERNAL_FILE_DENSE_MAP_LOCAL_ITERATOR_H
#define ENGINE_SOURCES_CONTAINERS_DENSE_MAP_INTERNAL_FILE_DENSE_MAP_LOCAL_ITERATOR_H

#include "../../PointerImitator.h"

#include <iterator>
#include <utility>

namespace egg::Containers::Internal
{
    template <typename IteratorType>
    class DenseMapLocalIterator final
    {
        template <typename>
        friend class DenseMapLocalIterator;

        using FirstType = decltype(std::as_const(std::declval<IteratorType>()->Value.first));
        using SecondType = decltype((std::declval<IteratorType>()->Value.second));

    public:
        using value_type = std::pair<FirstType, SecondType>;
        using pointer = PointerImitator<value_type>;
        using reference = value_type;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::input_iterator_tag;
        using iterator_concept = std::forward_iterator_tag;

        constexpr DenseMapLocalIterator() noexcept : Iterator {}, Offset {}
        {
        }

        constexpr DenseMapLocalIterator(IteratorType Iterator, const std::size_t Position) : Iterator { Iterator }, Offset { Position }
        {
        }

        template <typename OtherType>
            requires (!std::is_same_v<IteratorType, OtherType> && std::is_constructible_v<IteratorType, OtherType>)
        explicit constexpr DenseMapLocalIterator(const DenseMapLocalIterator<OtherType>& Other) noexcept
            : Iterator { Other.Iterator }, Offset { Other.Offset }
        {
        }

        constexpr DenseMapLocalIterator& operator++() noexcept
        {
            Offset = Iterator[Offset].Next;
            return *this;
        }

        constexpr DenseMapLocalIterator operator++(int) noexcept
        {
            DenseMapLocalIterator Original { *this };
            ++*this;
            return Original;
        }

        [[nodiscard]] constexpr pointer operator->() const noexcept
        {
            return pointer { operator*() };
        }

        [[nodiscard]] constexpr reference operator*() const noexcept
        {
            return {
                Iterator[Offset].Value.first,
                Iterator[Offset].Value.second
            };
        }

        [[nodiscard]] constexpr std::size_t GetIndex() const noexcept
        {
            return Offset;
        }

        template <typename OtherType>
        [[nodiscard]] constexpr bool operator==(const DenseMapLocalIterator<OtherType>& Other) const noexcept
        {
            return Offset == Other.Offset;
        }

        template <typename OtherType>
        [[nodiscard]] constexpr bool operator!=(const DenseMapLocalIterator<OtherType>& Other) const noexcept
        {
            return !(*this == Other);
        }

    private:
        IteratorType Iterator;
        std::size_t Offset;
    };
}

#endif // ENGINE_SOURCES_CONTAINERS_DENSE_MAP_INTERNAL_FILE_DENSE_MAP_LOCAL_ITERATOR_H
