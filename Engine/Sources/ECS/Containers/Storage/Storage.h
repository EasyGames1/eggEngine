#ifndef ENGINE_SOURCES_ECS_CONTAINERS_STORAGE_FILE_STORAGE_H
#define ENGINE_SOURCES_ECS_CONTAINERS_STORAGE_FILE_STORAGE_H

#include "../Container.h"
#include "../../../Containers/IterableAdaptor.h"
#include "../SparseSet/SparseSet.h"
#include "./Internal/StorageIterator.h"
#include "ECS/Entity.h"

#include <stdexcept>
#include <type_traits>
#include <utility>

namespace egg::ECS::Containers
{
    template <typename Type, ValidEntity EntityParameter, ValidAllocator<Type> AllocatorParameter = std::allocator<Type>>
    class Storage : public SparseSet<EntityParameter,
                                     typename AllocatorTraits<AllocatorParameter>::template rebind_alloc<EntityParameter>>
    {
        using StorageAllocatorTraits = AllocatorTraits<AllocatorParameter>;
        using ContainerType = PagedVector<Type, AllocatorParameter>;

    public:
        using BaseType = SparseSet<EntityParameter, typename StorageAllocatorTraits::template rebind_alloc<EntityParameter>>;
        using AllocatorType = AllocatorParameter;

        using ElementType = Type;
        using EntityType = EntityParameter;

        using Pointer = typename ContainerType::ConstPointer;
        using ConstPointer = typename ContainerType::ConstPointer;

        using Reference = typename ContainerType::Reference;
        using ConstReference = typename ContainerType::ConstReference;

        using Iterator = typename ContainerType::Iterator;
        using ConstIterator = typename ContainerType::ConstIterator;
        using ReverseIterator = typename ContainerType::ReverseIterator;
        using ConstReverseIterator = typename ContainerType::ConstReverseIterator;

        using EachIterator = Internal::StorageIterator<typename BaseType::Iterator, Iterator>;
        using ConstEachIterator = Internal::StorageIterator<typename BaseType::ConstIterator, ConstIterator>;
        using ReverseEachIterator = Internal::StorageIterator<typename BaseType::ReverseIterator, ReverseIterator>;
        using ConstReverseEachIterator = Internal::StorageIterator<typename BaseType::ConstReverseIterator, ConstReverseIterator>;

        using ElementIterable = egg::Containers::IterableAdaptor<Iterator>;
        using ConstElementIterable = egg::Containers::IterableAdaptor<ConstIterator>;
        using ReverseElementIterable = egg::Containers::IterableAdaptor<ReverseIterator>;
        using ConstReverseElementIterable = egg::Containers::IterableAdaptor<ConstReverseIterator>;

        using EachIterable = egg::Containers::IterableAdaptor<EachIterator>;
        using ConstEachIterable = egg::Containers::IterableAdaptor<ConstEachIterator>;
        using ReverseEachIterable = egg::Containers::IterableAdaptor<ReverseEachIterator>;
        using ConstReverseEachIterable = egg::Containers::IterableAdaptor<ConstReverseEachIterator>;


        Storage() : Storage { AllocatorType {} }
        {
        }

        explicit Storage(const AllocatorType& Allocator)
            noexcept(
                std::is_nothrow_constructible_v<BaseType, const AllocatorType&> &&
                std::is_nothrow_constructible_v<ContainerType, const AllocatorType&>)
            : BaseType { Allocator }, Payload { Allocator }
        {
        }

        Storage(Storage&& Other)
            noexcept(std::is_nothrow_move_constructible_v<BaseType> && std::is_nothrow_move_constructible_v<ContainerType>) = default;

        Storage(Storage&& Other, const AllocatorType& Allocator) : BaseType { std::move(Other), Allocator },
                                                                   Payload { std::move(Other.Payload), Allocator }
        {
            EGG_ASSERT(StorageAllocatorTraits::is_always_equal::value || Payload.GetAllocator() == Other.Payload.GetAllocator(),
                       "Cannot copy storage because it has a incompatible allocator");
        }

        ~Storage() noexcept override
        {
            ShrinkToSize(0u);
        }

        Storage& operator=(const Storage&) = delete;

        Storage& operator=(Storage&& Other)
            noexcept(std::is_nothrow_move_assignable_v<BaseType> && std::is_nothrow_move_assignable_v<ContainerType>)
        {
            EGG_ASSERT(StorageAllocatorTraits::is_always_equal::value || Payload.GetAllocator() == Other.Payload.GetAllocator(),
                       "Cannot copy storage because it has a incompatible allocator");
            ShrinkToSize(0u);
            BaseType::operator=(std::move(Other));
            Payload = std::move(Other.Payload);
            return *this;
        }

        template <typename... Args>
        Reference Emplace(const EntityType Entity, Args&&... Arguments)
        {
            if constexpr (std::is_aggregate_v<ElementType> && (sizeof...(Arguments) || !std::is_default_constructible_v<ElementType>))
            {
                const auto It { EmplaceElement(Entity, Type { std::forward<Args>(Arguments)... }) };
                return Payload.GetReference(It.GetIndex());
            }
            else
            {
                const auto It { EmplaceElement(Entity, std::forward<Args>(Arguments)...) };
                return Payload.GetReference(It.GetIndex());
            }
        }

        template <typename IteratorType>
        Iterator Insert(IteratorType First, IteratorType Last, ConstReference Value = {})
        {
            for (; First != Last; ++First)
            {
                EmplaceElement(*First, Value);
            }

            return Payload.Begin(BaseType::GetSize());
        }

        template <typename EntityIteratorType, typename ContainerIteratorType>
            requires (std::is_same_v<typename IteratorTraits<ContainerIteratorType>::value_type, ElementType>)
        Iterator Insert(EntityIteratorType First, EntityIteratorType Last, ContainerIteratorType From)
        {
            for (; First != Last; ++First, ++From)
            {
                EmplaceElement(*First, *From);
            }

            return Payload.Begin(BaseType::GetSize());
        }

        typename BaseType::Iterator Push(const EntityType Entity) override
        {
            if constexpr (std::is_default_constructible_v<ElementType>)
            {
                return EmplaceElement(Entity);
            }
            else
            {
                return BaseType::End();
            }
        }

        template <typename... Func>
        Reference Patch(const EntityType Entity, Func&&... Functions)
        {
            auto& Element { Payload.GetReference(BaseType::GetIndex(Entity)) };
            (std::forward<Func>(Functions)(Element), ...);
            return Element;
        }

        void Clear() override
        {
            AllocatorType Allocator { Payload.GetAllocator() };

            for (auto First = BaseType::Begin(), Last = BaseType::End(); First != Last; ++First)
            {
                BaseType::Erase(First);
                StorageAllocatorTraits::destroy(Allocator, std::addressof(Payload.GetReference(First.GetIndex())));
            }
        }

        void Reserve(const std::size_t Capacity) override
        {
            if (!Capacity) return;
            BaseType::Reserve(Capacity);
            Payload.Assure(Capacity - 1u);
        }

        void ShrinkToFit() override
        {
            BaseType::ShrinkToFit();
            ShrinkToSize(BaseType::GetSize());
        }

        [[nodiscard]] ElementIterable Element() noexcept
        {
            return { Payload.Begin(BaseType::GetSize()), Payload.End() };
        }

        [[nodiscard]] ConstElementIterable Element() const noexcept
        {
            return { Payload.Begin(BaseType::GetSize()), Payload.End() };
        }

        [[nodiscard]] ConstElementIterable ConstElement() const noexcept
        {
            return Element();
        }

        [[nodiscard]] ReverseElementIterable ReverseElement() noexcept
        {
            return { Payload.ReverseBegin(), Payload.ReverseEnd(BaseType::GetSize()) };
        }

        [[nodiscard]] ConstReverseElementIterable ReverseElement() const noexcept
        {
            return { Payload.ReverseBegin(), Payload.ReverseEnd(BaseType::GetSize()) };
        }

        [[nodiscard]] ConstReverseElementIterable ConstReverseElement() const noexcept
        {
            return ReverseElement();
        }

        [[nodiscard]] EachIterable Each() noexcept
        {
            return {
                EachIterator { BaseType::Begin(), Payload.Begin(BaseType::GetSize()) },
                EachIterator { BaseType::End(), Payload.End() }
            };
        }

        [[nodiscard]] ConstEachIterable Each() const noexcept
        {
            return {
                ConstEachIterator { BaseType::Begin(), Payload.Begin(BaseType::GetSize()) },
                ConstEachIterator { BaseType::End(), Payload.End() }
            };
        }

        [[nodiscard]] ConstEachIterable ConstEach() const noexcept
        {
            return Each();
        }

        [[nodiscard]] ReverseEachIterable ReverseEach() noexcept
        {
            return {
                ReverseEachIterator { BaseType::ReverseBegin(), Payload.ReverseBegin() },
                ReverseEachIterator { BaseType::ReverseEnd(), Payload.ReverseEnd(BaseType::GetSize()) }
            };
        }

        [[nodiscard]] ConstReverseEachIterable ReverseEach() const noexcept
        {
            return {
                ConstReverseEachIterator { BaseType::ReverseBegin(), Payload.ReverseBegin() },
                ConstReverseEachIterator { BaseType::ReverseEnd(), Payload.ReverseEnd(BaseType::GetSize()) }
            };
        }

        [[nodiscard]] ConstReverseEachIterable ConstReverseEach() const noexcept
        {
            return ReverseEach();
        }

        [[nodiscard]] Reference Get(const EntityType Entity) noexcept
        {
            return Payload.GetReference(BaseType::GetIndex(Entity));
        }

        [[nodiscard]] ConstReference Get(const EntityType Entity) const noexcept
        {
            return Payload.GetReference(BaseType::GetIndex(Entity));
        }

        [[nodiscard]] std::tuple<Reference> GetAsTuple(const EntityType Entity) noexcept
        {
            return std::forward_as_tuple(Get(Entity));
        }

        [[nodiscard]] std::tuple<ConstReference> GetAsTuple(const EntityType Entity) const noexcept
        {
            return std::forward_as_tuple(Get(Entity));
        }

        [[nodiscard]] std::size_t GetCapacity() const noexcept override
        {
            return Payload.GetExtent();
        }

    private:
        template <typename... Args>
        typename BaseType::Iterator EmplaceElement(const EntityType Entity, Args&&... Arguments)
        {
            const auto It { BaseType::Push(Entity) };

            try
            {
                std::uninitialized_construct_using_allocator(std::addressof(Payload.Assure(It.GetIndex())), Payload.GetAllocator(),
                                                             std::forward<Args>(Arguments)...);
            }
            catch (...)
            {
                BaseType::Erase(It);
                throw std::runtime_error("Failed to construct element");
            }

            return It;
        }

        void UpdateToPacked(const std::size_t Index, const std::size_t LeftIndex, const std::size_t RightIndex) override
        {
            BaseType::UpdateToPacked(Index, LeftIndex, RightIndex);
            SwapPayloadAt(LeftIndex, RightIndex);
        }

        void SwapAt(const std::size_t Left, const std::size_t Right) override
        {
            BaseType::SwapAt(Left, Right);
            SwapPayloadAt(Left, Right);
        }

        void SwapPayloadAt(const std::size_t Left, const std::size_t Right)
        {
            static_assert(std::is_move_constructible_v<Type> && std::is_move_assignable_v<Type>, "Non-movable type");
            std::swap(Payload.GetReference(Left), Payload.GetReference(Right));
        }

        void Pop(typename BaseType::Iterator First, typename BaseType::Iterator Last) override
        {
            for (AllocatorType Allocator { Payload.GetAllocator() }; First != Last; ++First)
            {
                auto& Element { Payload.GetReference(BaseType::GetIndex(*First)) };
                auto& Other { Payload.GetReference(BaseType::GetSize() - 1u) };
                Element = std::move(Other);
                StorageAllocatorTraits::destroy(Allocator, std::addressof(Other));
                BaseType::Erase(First);
            }
        }

        void ShrinkToSize(const std::size_t Size)
        {
            Payload.Shrink(Size, BaseType::GetSize());
        }

        ContainerType Payload;
    };

    template <typename Type, ValidEntity EntityParameter, ValidAllocator<Type> AllocatorParameter>
        requires std::is_same_v<Type, EntityParameter> || (!PageSizeTraits<Type>::value)
    class Storage<Type, EntityParameter, AllocatorParameter>
        : public SparseSet<EntityParameter, typename AllocatorTraits<AllocatorParameter>::template rebind_alloc<EntityParameter>>
    {
        using ContainerType = PagedVector<Type, AllocatorParameter>;

    public:
        using BaseType = SparseSet<EntityParameter, typename AllocatorTraits<AllocatorParameter>::template rebind_alloc<EntityParameter>>;
        using AllocatorType = AllocatorParameter;

        using ElementType = Type;
        using EntityType = EntityParameter;

        using Pointer = typename ContainerType::Pointer;
        using ConstPointer = typename ContainerType::ConstPointer;

        using Reference = typename ContainerType::Reference;
        using ConstReference = typename ContainerType::ConstReference;


        Storage() : Storage { AllocatorType {} }
        {
        }

        explicit Storage(const AllocatorType& Allocator) noexcept(std::is_nothrow_constructible_v<BaseType, const AllocatorType&>)
            : BaseType { Allocator }
        {
        }

        Storage(Storage&& Other) noexcept(std::is_nothrow_move_constructible_v<BaseType>) = default;

        Storage(Storage&& Other, const AllocatorType& Allocator) : BaseType { std::move(Other), Allocator }
        {
        }

        ~Storage() noexcept override = default;

        Storage& operator=(const Storage&) = delete;

        Storage& operator=(Storage&& Other) noexcept(std::is_nothrow_move_assignable_v<BaseType>) = default;
    };
}

#endif // ENGINE_SOURCES_ECS_CONTAINERS_STORAGE_FILE_STORAGE_H
