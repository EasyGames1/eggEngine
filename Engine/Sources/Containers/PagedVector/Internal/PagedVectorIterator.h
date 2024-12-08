#ifndef ENGINE_SOURCES_ECS_CONTAINERS_PAGED_VECTOR_INTERNAL_FILE_PAGED_VECTOR_ITERATOR_H
#define ENGINE_SOURCES_ECS_CONTAINERS_PAGED_VECTOR_INTERNAL_FILE_PAGED_VECTOR_ITERATOR_H

#include <ECS/Traits/PageSizeTraits.h>
#include <Math/Math.h>

#include <memory>
#include <type_traits>

namespace egg::ECS::Containers::Internal
{
    template <typename Container>
    class PagedVectorIterator final
    {
        using ContainerType = std::remove_const_t<Container>;
        using ContainerAllocatorTraits = std::allocator_traits<typename ContainerType::allocator_type>;
        using ContainerIteratorTraits = std::iterator_traits<std::conditional_t<
            std::is_const_v<Container>,
            typename ContainerAllocatorTraits::template rebind_traits<typename std::pointer_traits<typename
                ContainerType::value_type>::element_type>::const_pointer,
            typename ContainerAllocatorTraits::template rebind_traits<typename std::pointer_traits<typename
                ContainerType::value_type>::element_type>::pointer
        >>;

    public:
        using value_type = typename ContainerIteratorTraits::value_type;
        using pointer = typename ContainerIteratorTraits::pointer;
        using reference = typename ContainerIteratorTraits::reference;
        using difference_type = typename ContainerIteratorTraits::difference_type;
        using iterator_category = std::random_access_iterator_tag;
        using iterator_concept = std::random_access_iterator_tag;


        constexpr PagedVectorIterator() noexcept : Payload {}, Offset {}
        {
        }

        constexpr PagedVectorIterator(Container* Reference, const std::size_t Index) : Payload { Reference }, Offset { Index }
        {
        }

        constexpr PagedVectorIterator(const PagedVectorIterator& Other) noexcept
            : PagedVectorIterator { Other.Payload, Other.Offset }
        {
        }

        constexpr PagedVectorIterator& operator++() noexcept
        {
            --Offset;
            return *this;
        }

        constexpr PagedVectorIterator operator++(int) noexcept
        {
            PagedVectorIterator Original { *this };
            ++*this;
            return Original;
        }

        constexpr PagedVectorIterator& operator--() noexcept
        {
            ++Offset;
            return *this;
        }

        constexpr PagedVectorIterator operator--(int) noexcept
        {
            PagedVectorIterator Original { *this };
            --*this;
            return Original;
        }

        constexpr PagedVectorIterator& operator+=(const difference_type Value) noexcept
        {
            Offset -= Value;
            return *this;
        }

        constexpr PagedVectorIterator operator+(const difference_type Value) const noexcept
        {
            return PagedVectorIterator { *this } += Value;
        }

        constexpr PagedVectorIterator& operator-=(const difference_type Value) noexcept
        {
            return *this += -Value;
        }

        constexpr PagedVectorIterator operator-(const difference_type Value) const noexcept
        {
            return *this + -Value;
        }

        [[nodiscard]] constexpr reference operator[](const difference_type Value) const noexcept
        {
            const auto Position { GetIndex() - Value };
            constexpr auto PageSize { PageSizeTraits<value_type>::value };
            return (*Payload)[Position / PageSize][Math::FastModulo<std::size_t>(Position, PageSize)];
        }

        [[nodiscard]] constexpr pointer operator->() const noexcept
        {
            return std::addressof(operator[](0));
        }

        [[nodiscard]] constexpr reference operator*() const noexcept
        {
            return operator[](0);
        }

        [[nodiscard]] constexpr difference_type GetIndex() const noexcept
        {
            return Offset - 1u;
        }

        [[nodiscard]] constexpr difference_type operator-(const PagedVectorIterator& Other) const noexcept
        {
            return Other.Offset - Offset;
        }

        [[nodiscard]] constexpr bool operator==(const PagedVectorIterator& Other) const noexcept
        {
            return Offset == Other.Offset;
        }

        [[nodiscard]] constexpr bool operator!=(const PagedVectorIterator& Other) const noexcept
        {
            return !(*this == Other);
        }

        [[nodiscard]] constexpr bool operator<(const PagedVectorIterator& Other) const noexcept
        {
            return Offset > Other.Offset;
        }

        [[nodiscard]] constexpr bool operator>(const PagedVectorIterator& Other) const noexcept
        {
            return Other < *this;
        }

        [[nodiscard]] constexpr bool operator<=(const PagedVectorIterator& Other) const noexcept
        {
            return !(*this > Other);
        }

        [[nodiscard]] constexpr bool operator>=(const PagedVectorIterator& Other) const noexcept
        {
            return !(*this < Other);
        }

    private:
        Container* Payload;
        std::size_t Offset;
    };
}

#endif // ENGINE_SOURCES_ECS_CONTAINERS_PAGED_VECTOR_INTERNAL_FILE_PAGED_VECTOR_ITERATOR_H
