#ifndef ENGINE_SOURCES_ECS_CONTAINERS_PAGED_VECTOR_FILE_PAGED_VECTOR_H
#define ENGINE_SOURCES_ECS_CONTAINERS_PAGED_VECTOR_FILE_PAGED_VECTOR_H

#include "../Container.h"
#include "../../Traits/BasicEntityTraits.h"
#include "Containers/CompressedPair/CompressedPair.h"
#include "ECS/Traits/PageSizeTraits.h"
#include "./Internal/PagedVectorIterator.h"
#include "Math/Math.h"
#include "Type/Traits/Capabilities.h"

#include <memory>
#include <vector>

namespace egg::ECS::Containers
{
    template <typename Type, Types::ValidAllocator<Type> AllocatorParameter = std::allocator<Type>>
    class PagedVector
    {
        using ContainerAllocatorTraits = AllocatorTraits<AllocatorParameter>;

        using ContainerType = std::vector<typename ContainerAllocatorTraits::pointer,
                                          typename ContainerAllocatorTraits::template rebind_alloc<typename
                                              ContainerAllocatorTraits::pointer>>;
        using PayloadType = egg::Containers::CompressedPair<ContainerType, AllocatorParameter>;

        using PageSize = PageSizeTraits<Type>;

    public:
        using AllocatorType = AllocatorParameter;

        using ValueType = typename ContainerAllocatorTraits::value_type;

        using Pointer = typename ContainerAllocatorTraits::pointer;
        using ConstPointer = typename ContainerAllocatorTraits::const_pointer;

        using Reference = ValueType&;
        using ConstReference = const ValueType&;

        using Iterator = Internal::PagedVectorIterator<ContainerType>;
        using ConstIterator = Internal::PagedVectorIterator<const ContainerType>;
        using ReverseIterator = std::reverse_iterator<Iterator>;
        using ConstReverseIterator = std::reverse_iterator<ConstIterator>;


        constexpr explicit PagedVector(const AllocatorType& Allocator = {})
            noexcept(std::is_nothrow_constructible_v<PayloadType, const AllocatorType&, const AllocatorType&>)
            : Payload { Allocator, Allocator }
        {
        }

        constexpr PagedVector(PagedVector&& Other) noexcept(std::is_nothrow_move_constructible_v<PayloadType>) = default;

        constexpr PagedVector(PagedVector&& Other, const AllocatorType& Allocator)
            : Payload {
                std::piecewise_construct,
                std::forward_as_tuple(std::move(Other.Payload.GetFirst()), Allocator),
                std::forward_as_tuple(Allocator)
            }
        {
            EGG_ASSERT(ContainerAllocatorTraits::is_always_equal::value || GetAllocator() == Other.GetAllocator(),
                       "Cannot copy paged vector because it has a incompatible allocator");
        }

        constexpr ~PagedVector() noexcept
        {
            ReleasePages();
        }

        PagedVector& operator=(const PagedVector&) = delete;

        PagedVector& operator=(PagedVector&& Other) noexcept(std::is_nothrow_move_assignable_v<PayloadType>)
        {
            EGG_ASSERT(ContainerAllocatorTraits::is_always_equal::value || GetAllocator() == Other.GetAllocator(),
                       "Cannot copy paged vector because it has a incompatible allocator");
            ReleasePages();
            Payload = std::move(Other.Payload);
            return *this;
        }

        constexpr friend void swap(PagedVector& Left, PagedVector& Right) noexcept(std::is_nothrow_swappable_v<PayloadType>)
        {
            using std::swap;
            swap(Left.Payload, Right.Payload);
        }

        [[nodiscard]] constexpr std::size_t GetExtent() const noexcept
        {
            return Payload.GetFirst().size() * PageSize::value;
        }

        [[nodiscard]] constexpr AllocatorType GetAllocator() const noexcept
        {
            return Payload.GetSecond();
        }

        [[nodiscard]] constexpr Pointer GetPointer(const std::size_t Position) const
        {
            const auto Page { Position / PageSize::value };
            return Page < Payload.GetFirst().size() && Payload.GetFirst()[Page]
                       ? Payload.GetFirst()[Page] + Math::FastModulo(Position, PageSize::value)
                       : nullptr;
        }

        [[nodiscard]] constexpr Reference GetReference(const std::size_t Position) const
        {
            EGG_ASSERT(GetPointer(Position), "Payload not contain position");
            return Payload.GetFirst()[Position / PageSize::value][Math::FastModulo(Position, PageSize::value)];
        }

        [[nodiscard]] constexpr Iterator Begin(const std::size_t AvailableElements) noexcept
        {
            return { &Payload.GetFirst(), AvailableElements };
        }

        [[nodiscard]] constexpr ConstIterator Begin(const std::size_t AvailableElements) const noexcept
        {
            return { &Payload.GetFirst(), AvailableElements };
        }

        [[nodiscard]] constexpr ConstIterator ConstBegin(const std::size_t AvailableElements) const noexcept
        {
            return Begin(AvailableElements);
        }

        [[nodiscard]] constexpr Iterator End() noexcept
        {
            return { &Payload.GetFirst(), {} };
        }

        [[nodiscard]] constexpr ConstIterator End() const noexcept
        {
            return { &Payload.GetFirst(), {} };
        }

        [[nodiscard]] constexpr ConstIterator ConstEnd() const noexcept
        {
            return End();
        }

        [[nodiscard]] constexpr ReverseIterator ReverseBegin() noexcept
        {
            return std::make_reverse_iterator(End());
        }

        [[nodiscard]] constexpr ConstReverseIterator ReverseBegin() const noexcept
        {
            return std::make_reverse_iterator(End());
        }

        [[nodiscard]] constexpr ConstReverseIterator ConstReverseBegin() const noexcept
        {
            return ReverseBegin();
        }

        [[nodiscard]] constexpr ReverseIterator ReverseEnd(const std::size_t AvailableElements) noexcept
        {
            return std::make_reverse_iterator(Begin(AvailableElements));
        }

        [[nodiscard]] constexpr ConstReverseIterator ReverseEnd(const std::size_t AvailableElements) const noexcept
        {
            return std::make_reverse_iterator(Begin(AvailableElements));
        }

        [[nodiscard]] constexpr ConstReverseIterator ConstReverseEnd(const std::size_t AvailableElements) const noexcept
        {
            return ReverseEnd(AvailableElements);
        }

        constexpr Reference Assure(const std::size_t Position)
        {
            const auto Page { Position / PageSize::value };

            if (Page >= Payload.GetFirst().size()) Payload.GetFirst().resize(Page + 1u, nullptr);

            if (!Payload.GetFirst()[Page])
            {
                Payload.GetFirst()[Page] = ContainerAllocatorTraits::allocate(Payload.GetSecond(), PageSize::value);
                if constexpr (ValidEntity<Type>)
                {
                    std::uninitialized_fill(
                        Payload.GetFirst()[Page],
                        Payload.GetFirst()[Page] + PageSize::value,
                        EntityTraits<Type>::Tombstone
                    );
                }
            }

            return Payload.GetFirst()[Page][Math::FastModulo(Position, PageSize::value)];
        }

        constexpr void Shrink(const std::size_t Size, const std::size_t AvailableElements)
        {
            for (auto Position = Size; Position < AvailableElements; ++Position)
            {
                ContainerAllocatorTraits::destroy(Payload.GetSecond(), std::addressof(GetReference(Position)));
            }

            const auto From { (Size + PageSize::value - 1u) / PageSize::value };

            for (auto Position = From; Position < Payload.GetFirst().size(); ++Position)
            {
                ContainerAllocatorTraits::deallocate(Payload.GetSecond(), Payload.GetFirst()[Position], PageSize::value);
            }

            Payload.GetFirst().resize(From);
        }

    private:
        constexpr void ReleasePages()
        {
            for (auto&& Page : Payload.GetFirst())
            {
                if (!Page) continue;
                std::destroy(Page, Page + PageSize::value);
                ContainerAllocatorTraits::deallocate(Payload.GetSecond(), Page, PageSize::value);
                Page = nullptr;
            }
        }

        PayloadType Payload;
    };
}

#endif // ENGINE_SOURCES_ECS_CONTAINERS_PAGED_VECTOR_FILE_PAGED_VECTOR_H
