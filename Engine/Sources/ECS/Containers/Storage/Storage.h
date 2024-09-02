#ifndef ENGINE_SOURCES_ECS_CONTAINERS_STORAGE_FILE_STORAGE_H
#define ENGINE_SOURCES_ECS_CONTAINERS_STORAGE_FILE_STORAGE_H

#include "StorageIterator.h"
#include "../Container.h"
#include "../../../Containers/IterableAdaptor.h"
#include "../../Traits/ComponentTraits.h"
#include "../SparseSet/SparseSet.h"
#include "ECS/Entity.h"

#include <stdexcept>
#include <type_traits>
#include <utility>

namespace egg::ECS::Containers
{
    template <typename Type, typename EntityTypeParameter, typename AllocatorParameter = std::allocator<Type>>
    class Storage final : public SparseSet<EntityTypeParameter,
                                           typename Traits::Allocator<AllocatorParameter>::template rebind_alloc<Entity>>
    {
        using AllocatorTraits = Traits::Allocator<AllocatorParameter>;
        static_assert(ValidAllocator<Type, AllocatorParameter>, "Invalid allocator");

        using TraitsType = ComponentTraits<Type>;
        using EntityTraitsType = EntityTraits<EntityTypeParameter>;

        using ContainerType = PagedVector<Type, AllocatorParameter>;

    public:
        using BaseType = SparseSet<EntityTypeParameter, typename AllocatorTraits::template rebind_alloc<Entity>>;
        using AllocatorType = AllocatorParameter;

        using ElementType = Type;
        using EntityType = EntityTypeParameter;

        using Pointer = typename ContainerType::Pointer;
        using ConstPointer = typename ContainerType::ConstPointer;

        using Reference = typename ContainerType::Reference;
        using ConstReference = const typename ContainerType::Reference;

        using Iterator = typename ContainerType::Iterator;
        using ConstIterator = typename ContainerType::ConstIterator;
        using ReverseIterator = typename ContainerType::ReverseIterator;
        using ConstReverseIterator = typename ContainerType::ConstReverseIterator;

        using DataIterable = egg::Containers::IterableAdaptor<Iterator>;
        using ConstDataIterable = egg::Containers::IterableAdaptor<ConstIterator>;
        using ReverseDataIterable = egg::Containers::IterableAdaptor<ReverseIterator>;
        using ConstReverseDataIterable = egg::Containers::IterableAdaptor<ConstReverseIterator>;

        using EachIterable = egg::Containers::IterableAdaptor<StorageIterator<typename BaseType::Iterator, Iterator>>;
        using ConstEachIterable = egg::Containers::IterableAdaptor<StorageIterator<typename BaseType::ConstIterator, ConstIterator>>;
        using ReverseEachIterable = egg::Containers::IterableAdaptor<StorageIterator<typename BaseType::ReverseIterator, ReverseIterator>>;
        using ConstReverseEachIterable = egg::Containers::IterableAdaptor<StorageIterator<
            typename BaseType::ConstReverseIterator, ConstReverseIterator>>;


        Storage() : Storage { AllocatorType {} }
        {
        }

        explicit Storage(const AllocatorType& Allocator) : BaseType { Allocator }, Payload { Allocator }
        {
        }

        Storage(Storage&& Other) noexcept : BaseType { std::move(Other) }, Payload { std::move(Other.Payload) }
        {
        }

        Storage(Storage&& Other, const AllocatorType& Allocator) noexcept : BaseType { std::move(Other), Allocator },
                                                                            Payload { std::move(Other.Payload), Allocator }
        {
            EGG_ASSERT(AllocatorTraits::is_always_equal::value || Payload.GetAllocator() == Other.Payload.GetAllocator(),
                       "Cannot copy storage because it has a incompatible allocator");
        }

        ~Storage() override
        {
            ShrinkToSize(0u);
        }

        Storage& operator=(Storage&& Other) noexcept
        {
            EGG_ASSERT(AllocatorTraits::is_always_equal::value || Payload.GetAllocator() == Other.Payload.GetAllocator(),
                       "Cannot copy storage because it has a incompatible allocator");
            ShrinkToSize(0u);
            BaseType::operator=(std::move(Other));
            Payload = std::move(Other.Payload);
            return *this;
        }

        template <typename... Args>
        ElementType& Emplace(const EntityType Entity, Args&&... Arguments)
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
        Iterator Insert(IteratorType First, IteratorType Last, const ElementType& Value = {})
        {
            for (; First != Last; ++First)
            {
                EmplaceElement(*First, Value);
            }

            return Payload.Begin(BaseType::GetSize());
        }

        template <typename EntityIteratorType, typename ContainerIteratorType>
            requires (std::is_same_v<typename Traits::Iterator<ContainerIteratorType>::value_type, ElementType>)
        Iterator Insert(EntityIteratorType First, EntityIteratorType Last, ContainerIteratorType From)
        {
            for (; First != Last; ++First, ++From)
            {
                EmplaceElement(*First, *From);
            }

            return Payload.Begin(BaseType::GetSize());
        }

        template <typename... Func>
        ElementType& Patch(const EntityType Entity, Func&&... Functions)
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
                AllocatorTraits::destroy(Allocator, std::addressof(Payload.GetReference(First.GetIndex())));
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

        [[nodiscard]] DataIterable Data() noexcept
        {
            return { Payload.Begin(BaseType::GetSize()), Payload.End() };
        }

        [[nodiscard]] ConstDataIterable Data() const noexcept
        {
            return { Payload.Begin(BaseType::GetSize()), Payload.End() };
        }

        [[nodiscard]] ConstDataIterable ConstData() const noexcept
        {
            return Data();
        }

        [[nodiscard]] ReverseDataIterable ReverseData() noexcept
        {
            return { Payload.ReverseBegin(), Payload.ReverseEnd(BaseType::GetSize()) };
        }

        [[nodiscard]] ConstReverseDataIterable ReverseData() const noexcept
        {
            return { Payload.ReverseBegin(), Payload.ReverseEnd(BaseType::GetSize()) };
        }

        [[nodiscard]] ConstReverseDataIterable ConstReverseData() const noexcept
        {
            return ReverseData();
        }

        [[nodiscard]] EachIterable Each() noexcept
        {
            return {
                StorageIterator { BaseType::Begin(), Payload.Begin(BaseType::GetSize()) },
                StorageIterator { BaseType::End(), Payload.End() }
            };
        }

        [[nodiscard]] ConstEachIterable Each() const noexcept
        {
            return {
                StorageIterator { BaseType::Begin(), Payload.Begin(BaseType::GetSize()) },
                StorageIterator { BaseType::End(), Payload.End() }
            };
        }

        [[nodiscard]] ConstEachIterable ConstEach() const noexcept
        {
            return Each();
        }

        [[nodiscard]] ReverseEachIterable ReverseEach() noexcept
        {
            return {
                StorageIterator { BaseType::ReverseBegin(), Payload.ReverseBegin() },
                StorageIterator { BaseType::ReverseEnd(), Payload.ReverseEnd(BaseType::GetSize()) }
            };
        }

        [[nodiscard]] ConstReverseEachIterable ReverseEach() const noexcept
        {
            return {
                StorageIterator { BaseType::ReverseBegin(), Payload.ReverseBegin() },
                StorageIterator { BaseType::ReverseEnd(), Payload.ReverseEnd(BaseType::GetSize()) }
            };
        }

        [[nodiscard]] ConstReverseEachIterable ConstReverseEach() const noexcept
        {
            return ReverseEach();
        }

        [[nodiscard]] const ElementType& Get(const EntityType Entity) const noexcept
        {
            return Payload.GetReference(BaseType::GetIndex(Entity));
        }

        [[nodiscard]] ElementType& Get(const EntityType Entity) noexcept
        {
            return Payload.GetReference(BaseType::GetIndex(Entity));
        }

        [[nodiscard]] std::tuple<const ElementType&> GetAsTuple(const EntityType Entity) const noexcept
        {
            return std::forward_as_tuple(Get(Entity));
        }

        [[nodiscard]] std::tuple<ElementType&> GetAsTuple(const EntityType Entity) noexcept
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
                AllocatorTraits::destroy(Allocator, std::addressof(Other));
                BaseType::Erase(First);
            }
        }

        void ShrinkToSize(const std::size_t Size)
        {
            Payload.Shrink(Size, BaseType::GetSize());
        }

        ContainerType Payload;
    };

    template <typename EntityType, typename Alloc>
    class Storage<EntityType, EntityType, Alloc> : public SparseSet<EntityType, Alloc>
    {
        static_assert(ValidAllocator<EntityType, Alloc>, "Invalid allocator");
        using BaseType = SparseSet<EntityType, Alloc>;

    public:
        using AllocatorType = Alloc;

        Storage() : Storage { AllocatorType {} }
        {
        }

        explicit Storage(const AllocatorType& Allocator) : BaseType { Allocator }
        {
        }

        Storage(Storage&& Other) noexcept : BaseType { std::move(Other) }
        {
        }

        Storage(Storage&& Other, const AllocatorType& Allocator) noexcept : BaseType { std::move(Other), Allocator }
        {
        }

        ~Storage() override = default;

        Storage& operator=(Storage&& Other) noexcept
        {
            BaseType::operator=(std::move(Other));
            return *this;
        }
    };

    template <ZeroSizedComponent Type, typename EntityType, typename Alloc>
    class Storage<Type, EntityType, Alloc> final : public Storage<EntityType, Alloc>
    {
    public:
        using ElementType = Type;
    };
}

#endif // ENGINE_SOURCES_ECS_CONTAINERS_STORAGE_FILE_STORAGE_H
