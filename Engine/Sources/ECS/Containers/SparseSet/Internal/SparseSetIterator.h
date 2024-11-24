#ifndef ENGINE_SOURCES_ECS_CONTAINERS_SPARSE_SET_INTERNAL_FILE_SPARSE_SET_ITERATOR_H
#define ENGINE_SOURCES_ECS_CONTAINERS_SPARSE_SET_INTERNAL_FILE_SPARSE_SET_ITERATOR_H

#include <Config/Config.h>

#include <iterator>

namespace egg::ECS::Containers::Internal
{
    template <typename Container>
    class SparseSetIterator final
    {
    public:
        using value_type = typename Container::value_type;
        using pointer = typename Container::const_pointer;
        using reference = typename Container::const_reference;
        using difference_type = typename Container::difference_type;
        using iterator_category = std::random_access_iterator_tag;
        using iterator_concept = std::random_access_iterator_tag;


        constexpr SparseSetIterator() noexcept : Packed {}, Offset {}
        {
        }

        constexpr SparseSetIterator(const Container& Reference, const std::size_t Index) : Packed { &Reference }, Offset { Index }
        {
        }

        constexpr SparseSetIterator& operator++() noexcept
        {
            --Offset;
            return *this;
        }

        constexpr SparseSetIterator operator++(int) noexcept
        {
            SparseSetIterator Original { *this };
            ++*this;
            return Original;
        }

        constexpr SparseSetIterator& operator--() noexcept
        {
            ++Offset;
            return *this;
        }

        constexpr SparseSetIterator operator--(int) noexcept
        {
            SparseSetIterator Original { *this };
            --*this;
            return Original;
        }

        constexpr SparseSetIterator& operator+=(const difference_type Value) noexcept
        {
            //That's not UB: C++11(ISO/IEC 14882:2011) §4.7 Integral conversions [conv.integral/2]
            Offset -= Value;
            return *this;
        }

        constexpr SparseSetIterator operator+(const difference_type Value) const noexcept
        {
            return SparseSetIterator { *this } += Value;
        }

        constexpr SparseSetIterator& operator-=(const difference_type Value) noexcept
        {
            return *this += -Value;
        }

        constexpr SparseSetIterator operator-(const difference_type Value) const noexcept
        {
            return *this + -Value;
        }

        [[nodiscard]] constexpr reference operator[](const difference_type Value) const noexcept
        {
            const std::size_t Index { GetIndex() + Value };
            EGG_ASSERT(Index < Packed->size(), "Index out of bounds");
            return (*Packed)[Index];
        }

        [[nodiscard]] constexpr pointer operator->() const noexcept
        {
            return std::addressof(operator[](0));
        }

        [[nodiscard]] constexpr reference operator*() const noexcept
        {
            return operator[](0);
        }

        [[nodiscard]] constexpr pointer GetData() const noexcept
        {
            return Packed ? Packed->data() : nullptr;
        }

        [[nodiscard]] constexpr std::size_t GetIndex() const noexcept
        {
            return Offset - 1u;
        }

        [[nodiscard]] constexpr difference_type operator-(const SparseSetIterator& Other) const noexcept
        {
            return Other.Offset - Offset;
        }

        [[nodiscard]] constexpr bool operator==(const SparseSetIterator& Other) const noexcept
        {
            return Offset == Other.Offset;
        }

        [[nodiscard]] constexpr bool operator!=(const SparseSetIterator& Other) const noexcept
        {
            return !(*this == Other);
        }

        [[nodiscard]] constexpr bool operator<(const SparseSetIterator& Other) const noexcept
        {
            return Offset > Other.Offset;
        }

        [[nodiscard]] constexpr bool operator>(const SparseSetIterator& Other) const noexcept
        {
            return Other < *this;
        }

        [[nodiscard]] constexpr bool operator<=(const SparseSetIterator& Other) const noexcept
        {
            return !(*this > Other);
        }

        [[nodiscard]] constexpr bool operator>=(const SparseSetIterator& Other) const noexcept
        {
            return !(*this < Other);
        }

    private:
        const Container* Packed;
        std::size_t Offset;
    };
}

#endif // ENGINE_SOURCES_ECS_CONTAINERS_SPARSE_SET_INTERNAL_FILE_SPARSE_SET_ITERATOR_H
