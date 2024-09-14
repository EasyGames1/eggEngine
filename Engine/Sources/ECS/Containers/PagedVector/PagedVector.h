#ifndef ENGINE_SOURCES_ECS_CONTAINERS_PAGED_VECTOR_FILE_PAGED_VECTOR_H
#define ENGINE_SOURCES_ECS_CONTAINERS_PAGED_VECTOR_FILE_PAGED_VECTOR_H

#include "./PagedVectorIterator.h"
#include "../Container.h"
#include "../../Traits/BasicEntityTraits.h"
#include "ECS/Traits/PageSizeTraits.h"
#include "Math/Math.h"

#include <memory>
#include <vector>

namespace egg::ECS::Containers
{
    template <typename Type, ValidAllocator<Type> AllocatorParameter = std::allocator<Type>>
    class PagedVector final
    {
        using AllocatorTraits = AllocatorTraits<AllocatorParameter>;

        using ContainerType = std::vector<typename AllocatorTraits::pointer,
                                          typename AllocatorTraits::template rebind_alloc<typename AllocatorTraits::pointer>>;

        using PageSize = PageSizeTraits<Type>;

        void ReleasePages()
        {
            for (auto&& Page : Payload)
            {
                if (!Page) continue;
                std::destroy(Page, Page + PageSize::value);
                AllocatorTraits::deallocate(Allocator, Page, PageSize::value);
                Page = nullptr;
            }
        }

    public:
        using AllocatorType = AllocatorParameter;
        using ValueType = typename AllocatorTraits::value_type;
        using Pointer = typename AllocatorTraits::const_pointer;
        using ConstPointer = typename AllocatorTraits::const_pointer;
        using Reference = ValueType&;

        using Iterator = PagedVectorIterator<ContainerType>;
        using ConstIterator = PagedVectorIterator<const ContainerType>;
        using ReverseIterator = std::reverse_iterator<Iterator>;
        using ConstReverseIterator = std::reverse_iterator<ConstIterator>;


        explicit PagedVector(const AllocatorType& Allocator = {}) : Allocator { Allocator }
        {
        }

        PagedVector(PagedVector&& Other) noexcept : Payload { std::move(Other.Payload) }
        {
        }

        PagedVector(PagedVector&& Other, const AllocatorType& Allocator) : Payload { std::move(Other.Payload) }, Allocator { Allocator }
        {
            EGG_ASSERT(AllocatorTraits::is_always_equal::value || GetAllocator() == Other.GetAllocator(),
                       "Cannot copy paged vector because it has a incompatible allocator");
        }

        ~PagedVector() noexcept
        {
            ReleasePages();
        }

        PagedVector& operator=(const PagedVector&) = delete;

        PagedVector& operator=(PagedVector&& Other) noexcept
        {
            EGG_ASSERT(AllocatorTraits::is_always_equal::value || GetAllocator() == Other.GetAllocator(),
                       "Cannot copy paged vector because it has a incompatible allocator");
            ReleasePages();
            Payload = std::move(Other.Payload);
            Allocator = std::move(Other.Allocator);
            return *this;
        }

        [[nodiscard]] std::size_t GetExtent() const noexcept
        {
            return Payload.size() * PageSize::value;
        }

        [[nodiscard]] constexpr AllocatorType GetAllocator() const noexcept
        {
            return Allocator;
        }

        [[nodiscard]] Pointer GetPointer(const std::size_t Position) const
        {
            const auto Page { Position / PageSize::value };
            return Page < Payload.size() && Payload[Page]
                       ? Payload[Page] + Math::FastModulo(Position, PageSize::value)
                       : nullptr;
        }

        [[nodiscard]] Reference GetReference(const std::size_t Position) const
        {
            EGG_ASSERT(GetPointer(Position), "Payload not contain position");
            return Payload[Position / PageSize::value][Math::FastModulo(Position, PageSize::value)];
        }

        [[nodiscard]] Iterator Begin(const std::size_t AvailableElements) noexcept
        {
            return { &Payload, AvailableElements };
        }

        [[nodiscard]] ConstIterator Begin(const std::size_t AvailableElements) const noexcept
        {
            return { &Payload, AvailableElements };
        }

        [[nodiscard]] ConstIterator ConstBegin(const std::size_t AvailableElements) const noexcept
        {
            return Begin(AvailableElements);
        }

        [[nodiscard]] Iterator End() noexcept
        {
            return { &Payload, {} };
        }

        [[nodiscard]] ConstIterator End() const noexcept
        {
            return { &Payload, {} };
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

            if (Page >= Payload.size()) Payload.resize(Page + 1u, nullptr);

            if (!Payload[Page])
            {
                Payload[Page] = AllocatorTraits::allocate(Allocator, PageSize::value);
                if constexpr (ValidEntity<Type>)
                {
                    std::uninitialized_fill(Payload[Page], Payload[Page] + PageSize::value, EntityTraits<Type>::Tombstone);
                }
            }

            return Payload[Page][Math::FastModulo(Position, PageSize::value)];
        }

        void Shrink(const std::size_t Size, const std::size_t AvailableElements)
        {
            for (auto Position = Size; Position < AvailableElements; ++Position)
            {
                AllocatorTraits::destroy(Allocator, std::addressof(GetReference(Position)));
            }

            const auto From { (Size + PageSize::value - 1u) / PageSize::value };

            for (auto Position = From; Position < Payload.size(); ++Position)
            {
                AllocatorTraits::deallocate(Allocator, Payload[Position], PageSize::value);
            }

            Payload.resize(From);
        }

    private:
        ContainerType Payload;
        [[no_unique_address]] AllocatorType Allocator;
    };
}

#endif // ENGINE_SOURCES_ECS_CONTAINERS_PAGED_VECTOR_FILE_PAGED_VECTOR_H
