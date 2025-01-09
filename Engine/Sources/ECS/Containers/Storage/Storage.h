#ifndef ENGINE_SOURCES_ECS_CONTAINERS_STORAGE_FILE_STORAGE_H
#define ENGINE_SOURCES_ECS_CONTAINERS_STORAGE_FILE_STORAGE_H

#include "./Internal/StorageIterator.h"

#include <Containers/IterableAdaptor.h>
#include <Containers/PagedVector/PagedVector.h>
#include <ECS/Entity.h>
#include <ECS/Containers/Container.h>
#include <ECS/Containers/SparseSet/SparseSet.h>
#include <ECS/Traits/PageSizeTraits.h>
#include <Types/Capabilities/Capabilities.h>

#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace egg::ECS::Containers
{
    template <typename Element, typename Entity>
    concept OptimizableElement =
        ValidEntity<Entity> && (std::same_as<std::remove_const_t<Element>, Entity> || !PageSizeTraits<Element>::value);

    template <Types::Decayed Type, ValidEntity EntityParameter, Types::ValidAllocator<Type> AllocatorParameter = std::allocator<Type>>
    class Storage : public SparseSet<EntityParameter,
                                     typename AllocatorTraits<AllocatorParameter>::template rebind_alloc<EntityParameter>>
    {
        using ContainerAllocatorTraits = AllocatorTraits<AllocatorParameter>;
        using ContainerType = PagedVector<Type, AllocatorParameter>;

    public:
        using BaseType = SparseSet<EntityParameter, typename ContainerAllocatorTraits::template rebind_alloc<EntityParameter>>;
        using AllocatorType = AllocatorParameter;

        using ElementType = typename ContainerType::ValueType;
        using EntityType = typename BaseType::EntityType;

        using Pointer = typename ContainerType::ConstPointer;
        using ConstPointer = typename ContainerType::ConstPointer;

        using Reference = typename ContainerType::Reference;
        using ConstReference = typename ContainerType::ConstReference;

        using Iterator = typename ContainerType::Iterator;
        using ConstIterator = typename ContainerType::ConstIterator;
        using ReverseIterator = typename ContainerType::ReverseIterator;
        using ConstReverseIterator = typename ContainerType::ConstReverseIterator;

        using EntitiesIterator = typename BaseType::Iterator;
        using EntitiesConstIterator = typename BaseType::ConstIterator;
        using EntitiesReverseIterator = typename BaseType::ReverseIterator;
        using EntitiesConstReverseIterator = typename BaseType::ConstReverseIterator;

        using EachIterator = Internal::StorageIterator<EntitiesIterator, Iterator>;
        using EachConstIterator = Internal::StorageIterator<EntitiesConstIterator, ConstIterator>;
        using EachReverseIterator = Internal::StorageIterator<EntitiesReverseIterator, ReverseIterator>;
        using EachConstReverseIterator = Internal::StorageIterator<EntitiesConstReverseIterator, ConstReverseIterator>;

        using ElementsIterable = egg::Containers::IterableAdaptor<Iterator>;
        using ElementsConstIterable = egg::Containers::IterableAdaptor<ConstIterator>;
        using ElementsReverseIterable = egg::Containers::IterableAdaptor<ReverseIterator>;
        using ElementsConstReverseIterable = egg::Containers::IterableAdaptor<ConstReverseIterator>;

        using EachIterable = egg::Containers::IterableAdaptor<EachIterator>;
        using EachConstIterable = egg::Containers::IterableAdaptor<EachConstIterator>;
        using EachReverseIterable = egg::Containers::IterableAdaptor<EachReverseIterator>;
        using EachConstReverseIterable = egg::Containers::IterableAdaptor<EachConstReverseIterator>;


        constexpr Storage() : Storage { AllocatorType {} }
        {
        }

        constexpr explicit Storage(const AllocatorType& Allocator)
            noexcept(
                std::is_nothrow_constructible_v<BaseType, const AllocatorType&> &&
                std::is_nothrow_constructible_v<ContainerType, const AllocatorType&>)
            : BaseType { Allocator }, Payload { Allocator }
        {
        }

        Storage(const Storage&) = delete;

        constexpr Storage(Storage&& Other)
            noexcept(std::is_nothrow_move_constructible_v<BaseType> && std::is_nothrow_move_constructible_v<ContainerType>) = default;

        constexpr Storage(Storage&& Other, const AllocatorType& Allocator) : BaseType { std::move(Other), Allocator },
                                                                             Payload { std::move(Other.Payload), Allocator }
        {
            EGG_ASSERT(ContainerAllocatorTraits::is_always_equal::value || Payload.GetAllocator() == Other.Payload.GetAllocator(),
                       "Cannot move storage because it has an incompatible allocator");
        }

        constexpr ~Storage() noexcept override
        {
            ShrinkToSize(0u);
        }

        Storage& operator=(const Storage&) = delete;

        constexpr Storage& operator=(Storage&& Other)
            noexcept(std::is_nothrow_move_assignable_v<BaseType> && std::is_nothrow_move_assignable_v<ContainerType>)
        {
            EGG_ASSERT(ContainerAllocatorTraits::is_always_equal::value || Payload.GetAllocator() == Other.Payload.GetAllocator(),
                       "Cannot move storage because it has an incompatible allocator");
            ShrinkToSize(0u);
            BaseType::operator=(std::move(Other));
            Payload = std::move(Other.Payload);
            return *this;
        }

        constexpr friend void swap(Storage& Left, Storage& Right)
            noexcept(std::is_nothrow_swappable_v<BaseType> && std::is_nothrow_swappable_v<ContainerType>)
        {
            using std::swap;
            swap(static_cast<BaseType&>(Left), static_cast<BaseType&>(Right));
            swap(Left.Payload, Right.Payload);
        }

        template <typename... Args>
        constexpr ElementType& Emplace(const EntityType Entity, Args&&... Arguments)
        {
            if constexpr (std::is_aggregate_v<ElementType> && (sizeof...(Arguments) || !std::default_initializable<ElementType>))
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

        template <typename IteratorType, std::sentinel_for<IteratorType> SentinelType>
        constexpr Iterator Insert(IteratorType First, SentinelType Last, const ElementType& Value = {})
        {
            for (; First != Last; ++First)
            {
                EmplaceElement(*First, Value);
            }

            return ElementsBegin();
        }

        template <typename EntityIteratorType, typename ContainerIteratorType>
            requires (std::same_as<typename std::iterator_traits<ContainerIteratorType>::value_type, ElementType>)
        constexpr Iterator Insert(EntityIteratorType First, EntityIteratorType Last, ContainerIteratorType From)
        {
            for (; First != Last; ++First, ++From)
            {
                EmplaceElement(*First, *From);
            }

            return ElementsBegin();
        }

        template <typename... CallableTypes>
        constexpr ElementType& Patch(const EntityType Entity, CallableTypes&&... Callables)
        {
            auto& Element { Payload.GetReference(BaseType::GetIndex(Entity)) };
            (std::forward<CallableTypes>(Callables)(Element), ...);
            return Element;
        }

        constexpr void Clear() override
        {
            AllocatorType Allocator { Payload.GetAllocator() };

            for (auto First = BaseType::Begin(), Last = BaseType::End(); First != Last; ++First)
            {
                BaseType::Erase(First);
                ContainerAllocatorTraits::destroy(Allocator, std::addressof(Payload.GetReference(First.GetIndex())));
            }
        }

        constexpr void SwapElementsAt(const std::size_t Left, const std::size_t Right) override
        {
            BaseType::SwapElementsAt(Left, Right);
            SwapPayloadAt(Left, Right);
        }

        constexpr void Reserve(const std::size_t Capacity) override
        {
            if (!Capacity) return;
            BaseType::Reserve(Capacity);
            Payload.Assure(Capacity - 1u);
        }

        constexpr void ShrinkToFit() override
        {
            BaseType::ShrinkToFit();
            ShrinkToSize(BaseType::GetSize());
        }

        [[nodiscard]] constexpr Iterator ElementsBegin() noexcept
        {
            return Payload.Begin(BaseType::GetSize());
        }

        [[nodiscard]] constexpr ConstIterator ElementsBegin() const noexcept
        {
            return Payload.Begin(BaseType::GetSize());
        }

        [[nodiscard]] constexpr ConstIterator ElementsConstBegin() const noexcept
        {
            return ElementsBegin();
        }

        [[nodiscard]] constexpr Iterator ElementsEnd() noexcept
        {
            return Payload.End();
        }

        [[nodiscard]] constexpr ConstIterator ElementsEnd() const noexcept
        {
            return Payload.End();
        }

        [[nodiscard]] constexpr ConstIterator ElementsConstEnd() const noexcept
        {
            return ElementsEnd();
        }

        [[nodiscard]] constexpr ReverseIterator ElementsReverseBegin() noexcept
        {
            return Payload.ReverseBegin();
        }

        [[nodiscard]] constexpr ConstReverseIterator ElementsReverseBegin() const noexcept
        {
            return Payload.ReverseBegin();
        }

        [[nodiscard]] constexpr ConstReverseIterator ElementsConstReverseBegin() const noexcept
        {
            return ElementsReverseBegin();
        }

        [[nodiscard]] constexpr ReverseIterator ElementsReverseEnd() noexcept
        {
            return Payload.ReverseEnd(BaseType::GetSize());
        }

        [[nodiscard]] constexpr ConstReverseIterator ElementsReverseEnd() const noexcept
        {
            return Payload.ReverseEnd(BaseType::GetSize());
        }

        [[nodiscard]] constexpr ConstReverseIterator ElementsConstReverseEnd() const noexcept
        {
            return ElementsReverseEnd();
        }

        [[nodiscard]] constexpr ElementsIterable Elements() noexcept
        {
            return ElementsIterable { ElementsBegin(), ElementsEnd() };
        }

        [[nodiscard]] constexpr ElementsConstIterable Elements() const noexcept
        {
            return ElementsConstIterable { ElementsBegin(), ElementsEnd() };
        }

        [[nodiscard]] constexpr ElementsConstIterable ElementsConst() const noexcept
        {
            return Elements();
        }

        [[nodiscard]] constexpr ElementsReverseIterable ElementsReverse() noexcept
        {
            return ElementsReverseIterable { ElementsReverseBegin(), ElementsReverseEnd() };
        }

        [[nodiscard]] constexpr ElementsConstReverseIterable ElementsReverse() const noexcept
        {
            return ElementsConstReverseIterable { ElementsReverseBegin(), ElementsReverseEnd() };
        }

        [[nodiscard]] constexpr ElementsConstReverseIterable ElementsConstReverse() const noexcept
        {
            return ElementsReverse();
        }

        [[nodiscard]] constexpr EachIterator EachBegin() noexcept
        {
            return EachIterator { BaseType::Begin(), ElementsBegin() };
        }

        [[nodiscard]] constexpr EachConstIterator EachBegin() const noexcept
        {
            return EachConstIterator { BaseType::Begin(), ElementsBegin() };
        }

        [[nodiscard]] constexpr EachConstIterator EachConstBegin() const noexcept
        {
            return EachBegin();
        }

        [[nodiscard]] constexpr EachIterator EachEnd() noexcept
        {
            return EachIterator { BaseType::End(), ElementsEnd() };
        }

        [[nodiscard]] constexpr EachConstIterator EachEnd() const noexcept
        {
            return EachConstIterator { BaseType::End(), ElementsEnd() };
        }

        [[nodiscard]] constexpr EachConstIterator EachConstEnd() const noexcept
        {
            return EachEnd();
        }

        [[nodiscard]] constexpr EachReverseIterator EachReverseBegin() noexcept
        {
            return EachReverseIterator { BaseType::ReverseBegin(), ElementsReverseBegin() };
        }

        [[nodiscard]] constexpr EachConstReverseIterator EachReverseBegin() const noexcept
        {
            return EachConstReverseIterator { BaseType::ReverseBegin(), ElementsReverseBegin() };
        }

        [[nodiscard]] constexpr EachConstReverseIterator EachConstReverseBegin() const noexcept
        {
            return EachReverseBegin();
        }

        [[nodiscard]] constexpr EachReverseIterator EachReverseEnd() noexcept
        {
            return EachReverseIterator { BaseType::ReverseEnd(), ElementsReverseEnd() };
        }

        [[nodiscard]] constexpr EachConstReverseIterator EachReverseEnd() const noexcept
        {
            return EachConstReverseIterator { BaseType::ReverseEnd(), ElementsReverseEnd() };
        }

        [[nodiscard]] constexpr EachConstReverseIterator EachConstReverseEnd() const noexcept
        {
            return EachReverseEnd();
        }

        [[nodiscard]] constexpr EachIterable Each() noexcept
        {
            return EachIterable { EachBegin(), EachEnd() };
        }

        [[nodiscard]] constexpr EachConstIterable Each() const noexcept
        {
            return EachConstIterable { EachBegin(), EachEnd() };
        }

        [[nodiscard]] constexpr EachConstIterable EachConst() const noexcept
        {
            return Each();
        }

        [[nodiscard]] constexpr EachReverseIterable EachReverse() noexcept
        {
            return EachReverseIterable { EachReverseBegin(), EachReverseEnd() };
        }

        [[nodiscard]] constexpr EachConstReverseIterable EachReverse() const noexcept
        {
            return EachConstReverseIterable { EachReverseBegin(), EachReverseEnd() };
        }

        [[nodiscard]] constexpr EachConstReverseIterable EachConstReverse() const noexcept
        {
            return EachReverse();
        }

        [[nodiscard]] constexpr ElementType& Get(const EntityType Entity) noexcept
        {
            return Payload.GetReference(BaseType::GetIndex(Entity));
        }

        [[nodiscard]] constexpr const ElementType& Get(const EntityType Entity) const noexcept
        {
            return Payload.GetReference(BaseType::GetIndex(Entity));
        }

        [[nodiscard]] constexpr std::size_t GetCapacity() const noexcept override
        {
            return Payload.GetExtent();
        }

    protected:
        constexpr typename BaseType::Iterator TryEmplace(const EntityType Entity) override
        {
            if constexpr (std::default_initializable<ElementType>)
            {
                return EmplaceElement(Entity);
            }
            else
            {
                return BaseType::End();
            }
        }

        constexpr void Pop(typename BaseType::Iterator First, typename BaseType::Iterator Last) override
        {
            for (AllocatorType Allocator { Payload.GetAllocator() }; First != Last; ++First)
            {
                auto& Element { Payload.GetReference(BaseType::GetIndex(*First)) };
                auto& Other { Payload.GetReference(BaseType::GetSize() - 1u) };
                Element = std::move(Other);
                ContainerAllocatorTraits::destroy(Allocator, std::addressof(Other));
                BaseType::Erase(First);
            }
        }

    private:
        template <typename... Args>
        constexpr typename BaseType::Iterator EmplaceElement(const EntityType Entity, Args&&... Arguments)
        {
            const auto It { BaseType::TryEmplace(Entity) };

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

        constexpr void UpdateToPacked(const std::size_t Index, const std::size_t LeftIndex, const std::size_t RightIndex) override
        {
            BaseType::UpdateToPacked(Index, LeftIndex, RightIndex);
            SwapPayloadAt(LeftIndex, RightIndex);
        }

        constexpr void SwapPayloadAt(const std::size_t Left, const std::size_t Right)
        {
            static_assert(std::move_constructible<Type> && std::is_move_assignable_v<Type>, "Non-movable type");
            using std::swap;
            swap(Payload.GetReference(Left), Payload.GetReference(Right));
        }

        constexpr void ShrinkToSize(const std::size_t Size)
        {
            Payload.Shrink(Size, BaseType::GetSize());
        }

        ContainerType Payload;
    };


    template <Types::Decayed Type, ValidEntity EntityParameter, Types::ValidAllocator<Type> AllocatorParameter>
        requires OptimizableElement<Type, EntityParameter>
    class Storage<Type, EntityParameter, AllocatorParameter>
        : public SparseSet<EntityParameter, typename AllocatorTraits<AllocatorParameter>::template rebind_alloc<EntityParameter>>
    {
        using ContainerType = PagedVector<Type, AllocatorParameter>;

    public:
        using BaseType = SparseSet<EntityParameter, typename AllocatorTraits<AllocatorParameter>::template rebind_alloc<EntityParameter>>;
        using AllocatorType = AllocatorParameter;

        using ElementType = Type;
        using EntityType = EntityParameter;


        constexpr Storage() : Storage { AllocatorType {} }
        {
        }

        constexpr explicit Storage(const AllocatorType& Allocator) noexcept(std::is_nothrow_constructible_v<BaseType, const AllocatorType&>)
            : BaseType { Allocator }
        {
        }

        Storage(const Storage&) = delete;

        constexpr Storage(Storage&& Other) noexcept(std::is_nothrow_move_constructible_v<BaseType>) = default;

        constexpr Storage(Storage&& Other, const AllocatorType& Allocator) : BaseType { std::move(Other), Allocator }
        {
        }

        constexpr ~Storage() noexcept override = default;

        Storage& operator=(const Storage&) = delete;

        constexpr Storage& operator=(Storage&& Other) noexcept(std::is_nothrow_move_assignable_v<BaseType>) = default;
    };
}

#endif // ENGINE_SOURCES_ECS_CONTAINERS_STORAGE_FILE_STORAGE_H
