#ifndef ENGINE_SOURCES_EVENTS_FILE_DISPATCHER_H
#define ENGINE_SOURCES_EVENTS_FILE_DISPATCHER_H

#include <Containers/CompressedPair/CompressedPair.h>
#include <Containers/DenseMap/DenseMap.h>
#include <Events/EventLoop/EventLoop.h>
#include <Events/EventLoop/EventLoopInterface.h>
#include <Memory/Utils.h>
#include <Types/Types.h>
#include <Types/TypeInfo/TypeInfo.h>

#include <memory>
#include <type_traits>
#include <utility>

namespace egg::Events
{
    template <typename AllocatorParameter = std::allocator<void>>
    class Dispatcher
    {
        using KeyType = Types::IDType;
        using MappedType = std::shared_ptr<EventLoopInterface>;

        using DispatcherAllocatorTraits = std::allocator_traits<AllocatorParameter>;
        using ContainerAllocator = typename DispatcherAllocatorTraits::template rebind_alloc<std::pair<const KeyType, MappedType>>;
        using ContainerType = Containers::DenseMap<KeyType, MappedType, std::identity, std::equal_to<>, ContainerAllocator>;

        template <Types::Decayed Type>
        using EventLoopFor = EventLoop<Type, typename DispatcherAllocatorTraits::template rebind_alloc<Type>>;

        using EventLoopsType = Containers::CompressedPair<ContainerType, AllocatorParameter>;

    public:
        using AllocatorType = AllocatorParameter;

        template <Types::Decayed EventType>
        using SinkFor = typename EventLoopFor<EventType>::SinkType;


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

        template <Types::Decayed EventType>
        [[nodiscard]] constexpr SinkFor<EventType> GetSink(
            const Types::IDType Identifier = Types::TypeInfo<EventType>::template GetID<Dispatcher>()
        )
        {
            return Assure<EventType>(Identifier).GetSink();
        }

        template <Types::Decayed EventType, typename... Args>
        constexpr void Enqueue(Args&&... Arguments)
        {
            EnqueueWith<EventType>(
                Types::TypeInfo<EventType>::template GetID<Dispatcher>(),
                std::forward<Args>(Arguments)...
            );
        }

        template <typename EventType>
        constexpr void Enqueue(EventType&& Event)
        {
            EnqueueWith(
                Types::TypeInfo<std::decay_t<EventType>>::template GetID<Dispatcher>(),
                std::forward<EventType>(Event)
            );
        }

        template <Types::Decayed EventType, typename... Args>
        constexpr void EnqueueWith(const Types::IDType Identifier, Args&&... Arguments)
        {
            Assure<EventType>(Identifier).Enqueue(std::forward<Args>(Arguments)...);
        }

        template <typename EventType>
        constexpr void EnqueueWith(const Types::IDType Identifier, EventType&& Event)
        {
            Assure<std::decay_t<EventType>>(Identifier).Enqueue(std::forward<EventType>(Event));
        }

        template <typename EventType>
        constexpr void Trigger(EventType&& Event = {}) const
        {
            Trigger(
                Types::TypeInfo<std::decay_t<EventType>>::template GetID<Dispatcher>(),
                std::forward<EventType>(Event)
            );
        }

        template <typename EventType>
        constexpr void Trigger(const Types::IDType Identifier, EventType&& Event = {}) const
        {
            if (const auto* Loop { Find<std::decay_t<EventType>>(Identifier) })
            {
                Loop->Trigger(std::forward<EventType>(Event));
            }
        }

        template <Types::Decayed EventType>
        constexpr void Update(const Types::IDType Identifier = Types::TypeInfo<EventType>::template GetID<Dispatcher>())
        {
            if (const auto* Loop { Find<EventType>(Identifier) })
            {
                Loop->Publish();
            }
        }

        constexpr void Update() const
        {
            for (auto&& Loop : EventLoops.GetFirst())
            {
                Loop.second->Publish();
            }
        }

        template <Types::Decayed EventType>
        constexpr void Clear(const Types::IDType Identifier = Types::TypeInfo<EventType>::template GetID<Dispatcher>())
        {
            if (const auto* Loop { Find<EventType>(Identifier) })
            {
                Loop->Clear();
            }
        }

        constexpr void Clear() noexcept
        {
            for (auto&& Loop : EventLoops.GetFirst())
            {
                Loop.second->Clear();
            }
        }

        template <Types::Decayed EventType>
        [[nodiscard]] constexpr std::size_t GetSize(
            const Types::IDType Identifier = Types::TypeInfo<EventType>::template GetID<Dispatcher>()
        ) const noexcept
        {
            if (const auto* Loop { Find<EventType>(Identifier) })
            {
                return Loop->GetSize();
            }
            return 0u;
        }

        [[nodiscard]] constexpr std::size_t GetSize() const noexcept
        {
            std::size_t EventCount {};

            for (auto&& Loop : EventLoops.GetFirst())
            {
                EventCount += Loop.second->GetSize();
            }

            return EventCount;
        }

        constexpr AllocatorType GetAllocator() const noexcept
        {
            return EventLoops.GetSecond();
        }

    private:
        template <Types::Decayed EventType>
        [[nodiscard]] constexpr EventLoopFor<EventType>& Assure(const Types::IDType Identifier)
        {
            auto&& Pointer { EventLoops.GetFirst()[Identifier] };

            if (!Pointer)
            {
                const auto& Allocator { GetAllocator() };
                Pointer = std::allocate_shared<EventLoopFor<EventType>>(Allocator, Allocator);
            }

            return static_cast<EventLoopFor<EventType>&>(*Pointer);
        }

        template <Types::Decayed EventType>
        [[nodiscard]] constexpr const EventLoopFor<EventType>* Find(const Types::IDType Identifier) const
        {
            if (const auto It { EventLoops.GetFirst().Find(Identifier) }; It != EventLoops.GetFirst().End())
            {
                return static_cast<const EventLoopFor<EventType>*>(It->second.get());
            }

            return nullptr;
        }

        EventLoopsType EventLoops;
    };
}

#endif // ENGINE_SOURCES_EVENTS_FILE_DISPATCHER_H
