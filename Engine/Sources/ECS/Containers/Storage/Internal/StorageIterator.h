#ifndef ENGINE_SOURCES_ECS_CONTAINERS_STORAGE_INTERNAL_FILE_STORAGE_ITERATOR_H
#define ENGINE_SOURCES_ECS_CONTAINERS_STORAGE_INTERNAL_FILE_STORAGE_ITERATOR_H

#include "Containers/PointerImitator.h"

#include <tuple>

namespace egg::ECS::Containers::Internal
{
    template <typename IteratorParameter, typename... OtherTypes>
    class StorageIterator final
    {
    public:
        using IteratorType = IteratorParameter;
        using value_type = decltype(std::tuple_cat(std::make_tuple(*std::declval<IteratorType>()),
                                                   std::forward_as_tuple(*std::declval<OtherTypes>()...)));
        using pointer = egg::Containers::PointerImitator<value_type>;
        using reference = value_type;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::input_iterator_tag;
        using iterator_concept = std::random_access_iterator_tag;


        constexpr StorageIterator() : Iterators {}
        {
        }

        constexpr explicit StorageIterator(IteratorType Base, OtherTypes... Other) : Iterators { Base, Other... }
        {
        }

        template <typename... Args> requires
            std::conjunction_v<std::negation<std::is_same<OtherTypes, Args>>...> &&
            std::conjunction_v<std::is_constructible<OtherTypes, Args>...>
        explicit constexpr StorageIterator(const StorageIterator<IteratorType, Args...>& Other) : Iterators { Other.Iterators }
        {
        }

        constexpr StorageIterator& operator++() noexcept
        {
            ++std::get<IteratorType>(Iterators);
            (++std::get<OtherTypes>(Iterators), ...);
            return *this;
        }

        constexpr StorageIterator operator++(int) noexcept
        {
            StorageIterator Original { *this };
            ++*this;
            return Original;
        }

        constexpr StorageIterator& operator--() noexcept
        {
            --std::get<IteratorType>(Iterators);
            (--std::get<OtherTypes>(Iterators), ...);
            return *this;
        }

        constexpr StorageIterator operator--(int) noexcept
        {
            StorageIterator Original { *this };
            --*this;
            return Original;
        }

        constexpr StorageIterator& operator+=(const difference_type Value) noexcept
        {
            std::get<IteratorType>(Iterators) += Value;
            ((std::get<OtherTypes>(Iterators) += Value), ...);
            return *this;
        }

        constexpr StorageIterator operator+(const difference_type Value) const noexcept
        {
            return StorageIterator { *this } += Value;
        }

        constexpr StorageIterator& operator-=(const difference_type Value) noexcept
        {
            return *this += -Value;
        }

        constexpr StorageIterator operator-(const difference_type Value) const noexcept
        {
            return *this + -Value;
        }

        [[nodiscard]] constexpr reference operator[](const difference_type Value) const noexcept
        {
            return { std::get<IteratorType>(Iterators)[Value], std::get<OtherTypes>(Iterators)[Value]... };
        }

        [[nodiscard]] constexpr pointer operator->() const noexcept
        {
            return pointer { operator*() };
        }

        [[nodiscard]] constexpr reference operator*() const noexcept
        {
            return { *std::get<IteratorType>(Iterators), *std::get<OtherTypes>(Iterators)... };
        }

        [[nodiscard]] constexpr IteratorType Base() const noexcept
        {
            return std::get<IteratorType>(Iterators);
        }

        template <typename... Arguments>
        [[nodiscard]] constexpr difference_type operator-(const StorageIterator<Arguments...>& Other) const noexcept
        {
            return std::get<0>(Iterators) - std::get<0>(Other.Iterators);
        }

        template <typename... Arguments>
        [[nodiscard]] constexpr bool operator==(const StorageIterator<Arguments...>& Other) const noexcept
        {
            return std::get<0>(Iterators) == std::get<0>(Other.Iterators);
        }

        template <typename... Arguments>
        [[nodiscard]] constexpr bool operator!=(const StorageIterator<Arguments...>& Other) const noexcept
        {
            return !(*this == Other);
        }

        template <typename... Arguments>
        [[nodiscard]] constexpr bool operator<(const StorageIterator<Arguments...>& Other) const noexcept
        {
            return std::get<0>(Iterators) < std::get<0>(Other.Iterators);
        }

        template <typename... Arguments>
        [[nodiscard]] constexpr bool operator>(const StorageIterator<Arguments...>& Other) const noexcept
        {
            return Other < *this;
        }

        template <typename... Arguments>
        [[nodiscard]] constexpr bool operator<=(const StorageIterator<Arguments...>& Other) const noexcept
        {
            return !(*this > Other);
        }

        template <typename... Arguments>
        [[nodiscard]] constexpr bool operator>=(const StorageIterator<Arguments...>& Other) const noexcept
        {
            return !(*this < Other);
        }

    private:
        std::tuple<IteratorType, OtherTypes...> Iterators;
    };
}

#endif // ENGINE_SOURCES_ECS_CONTAINERS_STORAGE_INTERNAL_FILE_STORAGE_ITERATOR_H
