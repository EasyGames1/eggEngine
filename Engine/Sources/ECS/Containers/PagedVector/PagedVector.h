#ifndef ENGINE_SOURCES_ECS_CONTAINERS_PAGED_VECTOR_FILE_PAGED_VECTOR_H
#define ENGINE_SOURCES_ECS_CONTAINERS_PAGED_VECTOR_FILE_PAGED_VECTOR_H

#include "../Container.h"
#include "../../Traits/BasicEntityTraits.h"
#include "Containers/CompressedPair/CompressedPair.h"
#include "ECS/Traits/PageSizeTraits.h"
#include "./Internal/PagedVectorIterator.h"
#include "Math/Math.h"

#include <memory>
#include <vector>

namespace egg::ECS::Containers
{
    template <typename Type, ValidAllocator<Type> AllocatorParameter = std::allocator<Type>>
    class PagedVector
    {
        using AllocatorTraits = AllocatorTraits<AllocatorParameter>;

        using ContainerType = std::vector<typename AllocatorTraits::pointer,
                                          typename AllocatorTraits::template rebind_alloc<typename AllocatorTraits::pointer>>;
        using PayloadType = egg::Containers::CompressedPair<ContainerType, AllocatorParameter>;

        using PageSize = PageSizeTraits<Type>;

    public:
        using AllocatorType = AllocatorParameter;

        using ValueType = typename AllocatorTraits::value_type;

        using Pointer = typename AllocatorTraits::pointer;
        using ConstPointer = typename AllocatorTraits::const_pointer;

        using Reference = ValueType&;
        using ConstReference = const ValueType&;

        using Iterator = Internal::PagedVectorIterator<ContainerType>;
        using ConstIterator = Internal::PagedVectorIterator<const ContainerType>;
        using ReverseIterator = std::reverse_iterator<Iterator>;
        using ConstReverseIterator = std::reverse_iterator<ConstIterator>;


        explicit PagedVector(const AllocatorType& Allocator = {})
            noexcept(std::is_nothrow_constructible_v<PayloadType, const AllocatorType&, const AllocatorType&>)
            : Payload { Allocator, Allocator }
        {
        }

        PagedVector(PagedVector&& Other) noexcept(std::is_nothrow_move_constructible_v<PayloadType>) = default;

        PagedVector(PagedVector&& Other, const AllocatorType& Allocator)
            : Payload {
                std::piecewise_construct,
                std::forward_as_tuple(std::move(Other.Payload.GetFirst()), Allocator),
                std::forward_as_tuple(Allocator)
            }
        {
            EGG_ASSERT(AllocatorTraits::is_always_equal::value || GetAllocator() == Other.GetAllocator(),
                       "Cannot copy paged vector because it has a incompatible allocator");
        }

        ~PagedVector() noexcept
        {
            ReleasePages();
        }

        PagedVector& operator=(const PagedVector&) = delete;

        PagedVector& operator=(PagedVector&& Other) noexcept(std::is_nothrow_move_assignable_v<PayloadType>)
        {
            EGG_ASSERT(AllocatorTraits::is_always_equal::value || GetAllocator() == Other.GetAllocator(),
                       "Cannot copy paged vector because it has a incompatible allocator");
            ReleasePages();
            Payload = std::move(Other.Payload);
            return *this;
        }

        friend void swap(PagedVector& Left, PagedVector& Right) noexcept(std::is_nothrow_swappable_v<PayloadType>)
        {
            using std::swap;
            swap(Left.Payload, Right.Payload);
        }

        [[nodiscard]] std::size_t GetExtent() const noexcept
        {
            return Payload.GetFirst().size() * PageSize::value;
        }

        [[nodiscard]] constexpr AllocatorType GetAllocator() const noexcept
        {
            return Payload.GetSecond();
        }

        [[nodiscard]] Pointer GetPointer(const std::size_t Position) const
        {
            const auto Page { Position / PageSize::value };
            return Page < Payload.GetFirst().size() && Payload.GetFirst()[Page]
                       ? Payload.GetFirst()[Page] + Math::FastModulo(Position, PageSize::value)
                       : nullptr;
        }

        [[nodiscard]] Reference GetReference(const std::size_t Position) const
        {
            EGG_ASSERT(GetPointer(Position), "Payload not contain position");
            return Payload.GetFirst()[Position / PageSize::value][Math::FastModulo(Position, PageSize::value)];
        }

        [[nodiscard]] Iterator Begin(const std::size_t AvailableElements) noexcept
        {
            return { &Payload.GetFirst(), AvailableElements };
        }

        [[nodiscard]] ConstIterator Begin(const std::size_t AvailableElements) const noexcept
        {
            return { &Payload.GetFirst(), AvailableElements };
        }

        [[nodiscard]] ConstIterator ConstBegin(const std::size_t AvailableElements) const noexcept
        {
            return Begin(AvailableElements);
        }

        [[nodiscard]] Iterator End() noexcept
        {
            return { &Payload.GetFirst(), {} };
        }

        [[nodiscard]] ConstIterator End() const noexcept
        {
            return { &Payload.GetFirst(), {} };
        }

        [[nodiscard]] ConstIterator ConstEnd() const noexcept
        {
            return End();
        }

        [[nodiscard]] ReverseIterator ReverseBegin() noexcept
        {
            return std::make_reverse_iterator(End());
        }

        [[nodiscard]] ConstReverseIterator ReverseBegin() const noexcept
        {
            return std::make_reverse_iterator(End());
        }

        [[nodiscard]] ConstReverseIterator ConstReverseBegin() const noexcept
        {
            return ReverseBegin();
        }

        [[nodiscard]] ReverseIterator ReverseEnd(const std::size_t AvailableElements) noexcept
        {
            return std::make_reverse_iterator(Begin(AvailableElements));
        }

        [[nodiscard]] ConstReverseIterator ReverseEnd(const std::size_t AvailableElements) const noexcept
        {
            return std::make_reverse_iterator(Begin(AvailableElements));
        }

        [[nodiscard]] ConstReverseIterator ConstReverseEnd(const std::size_t AvailableElements) const noexcept
        {
            return ReverseEnd(AvailableElements);
        }

        Reference Assure(const std::size_t Position)
        {
            const auto Page { Position / PageSize::value };

            if (Page >= Payload.GetFirst().size()) Payload.GetFirst().resize(Page + 1u, nullptr);

            if (!Payload.GetFirst()[Page])
            {
                Payload.GetFirst()[Page] = AllocatorTraits::allocate(Payload.GetSecond(), PageSize::value);
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

        void Shrink(const std::size_t Size, const std::size_t AvailableElements)
        {
            for (auto Position = Size; Position < AvailableElements; ++Position)
            {
                AllocatorTraits::destroy(Payload.GetSecond(), std::addressof(GetReference(Position)));
            }

            const auto From { (Size + PageSize::value - 1u) / PageSize::value };

            for (auto Position = From; Position < Payload.GetFirst().size(); ++Position)
            {
                AllocatorTraits::deallocate(Payload.GetSecond(), Payload.GetFirst()[Position], PageSize::value);
            }

            Payload.GetFirst().resize(From);
        }

    private:
        void ReleasePages()
        {
            for (auto&& Page : Payload.GetFirst())
            {
                if (!Page) continue;
                std::destroy(Page, Page + PageSize::value);
                AllocatorTraits::deallocate(Payload.GetSecond(), Page, PageSize::value);
                Page = nullptr;
            }
        }

        PayloadType Payload;
    };
}

#endif // ENGINE_SOURCES_ECS_CONTAINERS_PAGED_VECTOR_FILE_PAGED_VECTOR_H
