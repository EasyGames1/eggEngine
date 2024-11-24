#ifndef ENGINE_SOURCES_CONTAINERS_DENSE_MAP_INTERNAL_FILE_DENSE_MAP_LOCAL_ITERATOR_H
#define ENGINE_SOURCES_CONTAINERS_DENSE_MAP_INTERNAL_FILE_DENSE_MAP_LOCAL_ITERATOR_H

#include <Containers/PointerImitator.h>
#include <Types/Traits/Constness.h>

#include <iterator>
#include <utility>

namespace egg::Containers::Internal
{
    template <typename IteratorType>
    class DenseMapLocalIterator final
    {
        template <typename>
        friend class DenseMapLocalIterator;

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
        using iterator_concept = std::forward_iterator_tag;


        constexpr DenseMapLocalIterator() noexcept : Iterator {}, Offset {}
        {
        }

        constexpr DenseMapLocalIterator(IteratorType Iterator, const std::size_t Position) : Iterator { Iterator }, Offset { Position }
        {
        }

        template <std::convertible_to<IteratorType> OtherIteratorType> requires (!std::same_as<OtherIteratorType, IteratorType>)
        constexpr explicit DenseMapLocalIterator(const DenseMapLocalIterator<OtherIteratorType>& Other) noexcept
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

        template <std::convertible_to<IteratorType> OtherIteratorType>
        [[nodiscard]] constexpr bool operator==(const DenseMapLocalIterator<OtherIteratorType>& Other) const noexcept
        {
            return Offset == Other.Offset;
        }

        template <std::convertible_to<IteratorType> OtherIteratorType>
        [[nodiscard]] constexpr bool operator!=(const DenseMapLocalIterator<OtherIteratorType>& Other) const noexcept
        {
            return !(*this == Other);
        }

    private:
        IteratorType Iterator;
        std::size_t Offset;
    };
}

#endif // ENGINE_SOURCES_CONTAINERS_DENSE_MAP_INTERNAL_FILE_DENSE_MAP_LOCAL_ITERATOR_H
