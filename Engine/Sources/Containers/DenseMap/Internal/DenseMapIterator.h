#ifndef ENGINE_SOURCES_CONTAINERS_DENSE_MAP_INTERNAL_FILE_DENSE_MAP_ITERATOR_H
#define ENGINE_SOURCES_CONTAINERS_DENSE_MAP_INTERNAL_FILE_DENSE_MAP_ITERATOR_H

#include <Containers/PointerImitator.h>
#include <Types/Constness.h>

#include <iterator>
#include <utility>

namespace egg::Containers::Internal
{
    template <typename IteratorType>
    class DenseMapIterator final
    {
        template <typename>
        friend class DenseMapIterator;

        using IteratorPairType = typename std::iter_value_t<IteratorType>::ValueType;
        using FirstType = std::add_lvalue_reference_t<std::add_const_t<typename IteratorPairType::first_type>>;
        using SecondType = std::add_lvalue_reference_t<Types::ConstnessAs<
            std::remove_reference_t<std::iter_reference_t<IteratorType>>,
            typename IteratorPairType::second_type
        >>;

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

        template <std::convertible_to<IteratorType> OtherIteratorType> requires (!std::same_as<OtherIteratorType, IteratorType>)
        constexpr explicit DenseMapIterator(const DenseMapIterator<OtherIteratorType>& Other) noexcept : Iterator { Other.Iterator }
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

        template <std::convertible_to<IteratorType> OtherIteratorType>
        [[nodiscard]] constexpr difference_type operator-(const DenseMapIterator<OtherIteratorType>& Other) const noexcept
        {
            return Iterator - Other.Iterator;
        }

        template <std::convertible_to<IteratorType> OtherIteratorType>
        [[nodiscard]] constexpr bool operator==(const DenseMapIterator<OtherIteratorType>& Other) const noexcept
        {
            return Iterator == Other.Iterator;
        }

        template <std::convertible_to<IteratorType> OtherIteratorType>
        [[nodiscard]] constexpr bool operator!=(const DenseMapIterator<OtherIteratorType>& Other) const noexcept
        {
            return !(*this == Other);
        }

        template <std::convertible_to<IteratorType> OtherIteratorType>
        [[nodiscard]] constexpr bool operator<(const DenseMapIterator<OtherIteratorType>& Other) const noexcept
        {
            return Iterator < Other.Iterator;
        }

        template <std::convertible_to<IteratorType> OtherIteratorType>
        [[nodiscard]] constexpr bool operator>(const DenseMapIterator<OtherIteratorType>& Other) const noexcept
        {
            return Other < *this;
        }

        template <std::convertible_to<IteratorType> OtherIteratorType>
        [[nodiscard]] constexpr bool operator<=(const DenseMapIterator<OtherIteratorType>& Other) const noexcept
        {
            return !(*this > Other);
        }

        template <std::convertible_to<IteratorType> OtherIteratorType>
        [[nodiscard]] constexpr bool operator>=(const DenseMapIterator<OtherIteratorType>& Other) const noexcept
        {
            return !(*this < Other);
        }

    private:
        IteratorType Iterator;
    };
}

#endif // ENGINE_SOURCES_CONTAINERS_DENSE_MAP_INTERNAL_FILE_DENSE_MAP_ITERATOR_H
