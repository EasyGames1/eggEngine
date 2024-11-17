#ifndef ENGINE_SOURCES_EVENTS_FILE_DISPATCHER_H
#define ENGINE_SOURCES_EVENTS_FILE_DISPATCHER_H

#include "./EventLoop.h"
#include "./EventLoopInterface.h"
#include "Containers/CompressedPair/CompressedPair.h"
#include "Containers/DenseMap/DenseMap.h"
#include "Memory/Utils.h"
#include "Type/TypeInfo.h"
#include "Type/Types.h"

#include <memory>
#include <type_traits>
#include <utility>

namespace egg::Events
{
    template <typename AllocatorParameter>
    class Dispatcher
    {
        using KeyType = Types::TypeID;
        using MappedType = std::unique_ptr<EventLoopInterface, Memory::Deleter<AllocatorParameter>>;

        using DispatcherAllocatorTraits = std::allocator_traits<AllocatorParameter>;
        using ContainerAllocator = typename DispatcherAllocatorTraits::template rebind_alloc<std::pair<const KeyType, MappedType>>;
        using ContainerType = Containers::DenseMap<KeyType, MappedType, std::identity, std::equal_to<>, ContainerAllocator>;

        template <typename Type>
        using EventLoopType = EventLoop<Type, typename DispatcherAllocatorTraits::template rebind_alloc<Type>>;

        using EventLoopsType = Containers::CompressedPair<ContainerType, AllocatorParameter>;

    public:
        using AllocatorType = AllocatorParameter;

        template <typename EventType>
        using SinkType = typename EventLoopType<EventType>::SinkType;


        constexpr Dispatcher() : Dispatcher { AllocatorType {} }
        {
        }

        constexpr explicit Dispatcher(const AllocatorType& Allocator) : EventLoops { Allocator, Allocator }
        {
        }

        constexpr Dispatcher(Dispatcher&&) noexcept(std::is_nothrow_move_constructible_v<EventLoopsType>) = default;

        constexpr Dispatcher(Dispatcher&& Other, const AllocatorType& Allocator)
            : EventLoops {
                std::piecewise_construct,
                std::forward_as_tuple(std::move(Other.EventLoops.GetFirst()), Allocator),
                std::forward_as_tuple(Allocator)
            }
        {
            EGG_ASSERT(DispatcherAllocatorTraits::is_always_equal::value || GetAllocator() == Other.GetAllocator(),
                       "Cannot move dispatcher because it has an incompatible allocator");
        }

        constexpr Dispatcher& operator=(Dispatcher&& Other) noexcept(std::is_nothrow_move_assignable_v<EventLoopsType>)
        {
            EGG_ASSERT(DispatcherAllocatorTraits::is_always_equal::value || GetAllocator() == Other.GetAllocator(),
                       "Cannot move dispatcher because it has an incompatible allocator");
            EventLoops = std::move(Other.EventLoops);
            return *this;
        }

        constexpr friend void swap(Dispatcher& Left, Dispatcher& Right) noexcept(std::is_nothrow_swappable_v<EventLoopsType>)
        {
            using std::swap;
            swap(Left.EventLoops, Right.EventLoops);
        }

        template <typename Type>
        [[nodiscard]] constexpr std::size_t GetSize(
            const Types::TypeID Identifier = Types::TypeInfo<Type>::template GetID<Dispatcher>()
        ) const noexcept
        {
            if (const auto* Loop { Assure<std::decay_t<Type>>(Identifier) })
            {
                return Loop->GetSize();
            }
            return 0u;
        }

        [[nodiscard]] constexpr std::size_t GetSize() const noexcept
        {
            std::size_t Size {};

            for (auto&& Loop : EventLoops.GetFirst())
            {
                Size += Loop.second->GetSize();
            }

            return Size;
        }

        template <typename Type>
        [[nodiscard]] constexpr SinkType<std::decay_t<Type>> GetSink(
            const Types::TypeID Identifier = Types::TypeInfo<Type>::template GetID<Dispatcher>()
        )
        {
            return Assure<std::decay_t<Type>>(Identifier).GetSink();
        }

        template <typename Type>
        constexpr void Trigger(Type&& Event = {}) const
        {
            Trigger(Types::TypeInfo<std::decay_t<Type>>::template GetID<Dispatcher>(), std::forward<Type>(Event));
        }

        template <typename Type>
        constexpr void Trigger(const Types::TypeID Identifier, Type&& Event = {}) const
        {
            if (const auto* Loop { Assure<std::decay_t<Type>>(Identifier) })
            {
                Loop->Trigger(std::forward<Type>(Event));
            }
        }

        template <typename Type, typename... Args>
        constexpr void Enqueue(Args&&... Arguments)
        {
            EnqueueWith<std::decay_t<Type>>(Types::TypeInfo<Type>::template GetID<Dispatcher>(), std::forward<Args>(Arguments)...);
        }

        template <typename Type>
        constexpr void Enqueue(Type&& Event)
        {
            EnqueueWith(Types::TypeInfo<std::decay_t<Type>>::template GetID<Dispatcher>(), std::forward<Type>(Event));
        }

        template <Types::Decayed Type, typename... Args>
        constexpr void EnqueueWith(const Types::TypeID Identifier, Args&&... Arguments)
        {
            Assure<Type>(Identifier).Enqueue(std::forward<Args>(Arguments)...);
        }

        template <typename Type>
        constexpr void EnqueueWith(const Types::TypeID Identifier, Type&& Event)
        {
            Assure<std::decay_t<Type>>(Identifier).Enqueue(std::forward<Type>(Event));
        }

        template <typename Type>
        constexpr void Clear(const Types::TypeID Identifier = Types::TypeInfo<Type>::template GetID<Dispatcher>())
        {
            Assure<std::decay_t<Type>>(Identifier).Clear();
        }

        constexpr void Clear() noexcept
        {
            for (auto&& Loop : EventLoops.GetFirst())
            {
                Loop.second->Clear();
            }
        }

        template <typename Type>
        constexpr void Update(const Types::TypeID Identifier = Types::TypeInfo<Type>::template GetID<Dispatcher>())
        {
            Assure<std::decay_t<Type>>(Identifier).Publish();
        }

        constexpr void Update() const
        {
            for (auto&& Loop : EventLoops.GetFirst())
            {
                Loop.second->Publish();
            }
        }

        constexpr AllocatorType GetAllocator() const noexcept
        {
            return EventLoops.GetSecond();
        }

    private:
        template <Types::Decayed Type>
        [[nodiscard]] constexpr EventLoopType<Type>& Assure(const Types::TypeID Identifier)
        {
            auto&& Pointer { EventLoops.GetFirst()[Identifier] };

            if (!Pointer)
            {
                const AllocatorType Allocator { GetAllocator() };
                Pointer = Memory::AllocateUnique<EventLoopType<Type>>(Allocator, Allocator);
            }

            return static_cast<EventLoopType<Type>&>(*Pointer);
        }

        template <Types::Decayed Type>
        [[nodiscard]] constexpr const EventLoopType<Type>* Assure(const Types::TypeID Identifier) const
        {
            if (auto It = EventLoops.GetFirst().Find(Identifier); It != EventLoops.GetFirst().ConstEnd())
            {
                return static_cast<const EventLoopType<Type>*>(It->second.get());
            }
            return nullptr;
        }

        EventLoopsType EventLoops;
    };
}

#endif // ENGINE_SOURCES_EVENTS_FILE_DISPATCHER_H
