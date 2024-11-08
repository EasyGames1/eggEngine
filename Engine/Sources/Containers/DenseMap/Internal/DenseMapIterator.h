#ifndef ENGINE_SOURCES_CONTAINERS_DENSE_MAP_INTERNAL_FILE_DENSE_MAP_ITERATOR_H
#define ENGINE_SOURCES_CONTAINERS_DENSE_MAP_INTERNAL_FILE_DENSE_MAP_ITERATOR_H

#include "../../PointerImitator.h"

#include <iterator>
#include <utility>

namespace egg::Containers::Internal
{
    template <typename IteratorType>
    class DenseMapIterator final
    {
        template <typename>
        friend class DenseMapIterator;

        using FirstType = decltype(std::as_const(std::declval<IteratorType>()->Value.first));
        using SecondType = decltype((std::declval<IteratorType>()->Value.second));

    public:
        using value_type = std::pair<FirstType, SecondType>;
        using pointer = PointerImitator<value_type>;
        using reference = value_type;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::input_iterator_tag;
        using iterator_concept = std::random_access_iterator_tag;

        constexpr DenseMapIterator() noexcept : Iterator {}
        {
        }

        constexpr explicit DenseMapIterator(const IteratorType Iterator) : Iterator { Iterator }
        {
        }

        template <std::convertible_to<IteratorType> OtherType> requires (!std::same_as<IteratorType, OtherType>)
        constexpr explicit DenseMapIterator(const DenseMapIterator<OtherType>& Other) noexcept : Iterator { Other.Iterator }
        {
        }

        constexpr DenseMapIterator& operator++() noexcept
        {
            ++Iterator;
            return *this;
        }

        constexpr DenseMapIterator operator++(int) noexcept
        {
            DenseMapIterator Original { *this };
            ++*this;
            return Original;
        }

        constexpr DenseMapIterator& operator--() noexcept
        {
            --Iterator;
            return *this;
        }

        constexpr DenseMapIterator operator--(int) noexcept
        {
            DenseMapIterator Original { *this };
            --*this;
            return Original;
        }

        constexpr DenseMapIterator& operator+=(const difference_type Value) noexcept
        {
            Iterator += Value;
            return *this;
        }

        constexpr DenseMapIterator operator+(const difference_type Value) const noexcept
        {
            return DenseMapIterator { *this } += Value;
        }

        constexpr DenseMapIterator& operator-=(const difference_type Value) noexcept
        {
            return *this += -Value;
        }

        constexpr DenseMapIterator operator-(const difference_type Value) const noexcept
        {
            return *this + -Value;
        }

        [[nodiscard]] constexpr reference operator[](const difference_type Value) const noexcept
        {
            return { Iterator[Value].Value.first, Iterator[Value].Value.second };
        }

        [[nodiscard]] constexpr pointer operator->() const noexcept
        {
            return pointer { operator*() };
        }

        [[nodiscard]] constexpr reference operator*() const noexcept
        {
            return operator[](0);
        }

        template <typename OtherIteratorType>
        [[nodiscard]] constexpr difference_type operator-(const DenseMapIterator<OtherIteratorType>& Other) const noexcept
        {
            return Iterator - Other.Iterator;
        }

        template <typename OtherIteratorType>
        [[nodiscard]] constexpr bool operator==(const DenseMapIterator<OtherIteratorType>& Other) const noexcept
        {
            return Iterator == Other.Iterator;
        }

        template <typename OtherIteratorType>
        [[nodiscard]] constexpr bool operator!=(const DenseMapIterator<OtherIteratorType>& Other) const noexcept
        {
            return !(*this == Other);
        }

        template <typename OtherIteratorType>
        [[nodiscard]] constexpr bool operator<(const DenseMapIterator<OtherIteratorType>& Other) const noexcept
        {
            return Iterator < Other.Iterator;
        }

        template <typename OtherIteratorType>
        [[nodiscard]] constexpr bool operator>(const DenseMapIterator<OtherIteratorType>& Other) const noexcept
        {
            return Other < *this;
        }

        template <typename OtherIteratorType>
        [[nodiscard]] constexpr bool operator<=(const DenseMapIterator<OtherIteratorType>& Other) const noexcept
        {
            return !(*this > Other);
        }

        template <typename OtherIteratorType>
        [[nodiscard]] constexpr bool operator>=(const DenseMapIterator<OtherIteratorType>& Other) const noexcept
        {
            return !(*this < Other);
        }

    private:
        IteratorType Iterator;
    };
}

#endif // ENGINE_SOURCES_CONTAINERS_DENSE_MAP_INTERNAL_FILE_DENSE_MAP_ITERATOR_H
