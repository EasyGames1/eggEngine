#ifndef ENGINE_SOURCES_EVENTS_FILE_EVENT_LOOP_H
#define ENGINE_SOURCES_EVENTS_FILE_EVENT_LOOP_H

#include <Events/EventLoopInterface.h>
#include <Events/Signal.h>
#include <Events/Sink.h>
#include <Types/Capabilities/Capabilities.h>

#include <concepts>
#include <iterator>
#include <memory>
#include <vector>

namespace egg::Events
{
    template <Types::Decayed Type, Types::ValidAllocator<Type> AllocatorParameter = std::allocator<Type>>
    class EventLoop final : public EventLoopInterface
    {
        using LoopAllocatorTraits = std::allocator_traits<AllocatorParameter>;

        using CallbackSignature = void(Type&);
        using SignalType = Signal<CallbackSignature, typename LoopAllocatorTraits::template rebind_alloc<Delegate<CallbackSignature>>>;

        using ContainerType = std::vector<Type, AllocatorParameter>;

    public:
        using AllocatorType = AllocatorParameter;

        using SinkType = Sink<SignalType>;
        using EventType = Type;


        constexpr EventLoop()
            noexcept(
                std::is_nothrow_default_constructible_v<SignalType> &&
                std::is_nothrow_default_constructible_v<ContainerType>)
        = default;

        constexpr explicit EventLoop(const AllocatorType& Allocator)
            noexcept(std::is_nothrow_constructible_v<SignalType, const AllocatorType&>)
            : Signal { Allocator }, Events { Allocator }
        {
        }

        constexpr void Publish() override
        {
            const std::size_t EventCount { GetSize() };

            for (std::size_t Position = 0u; Position < EventCount; ++Position)
            {
                Signal.Publish(Events[Position]);
            }

            Events.erase(Events.cbegin(), std::next(Events.cbegin(), EventCount));
        }

        constexpr void Trigger(EventType Event) const
        {
            Signal.Publish(Event);
        }

        template <typename... Args>
        constexpr void Enqueue(Args&&... Arguments)
        {
            if constexpr (std::is_aggregate_v<EventType> && (sizeof...(Args) != 0u || !std::default_initializable<EventType>))
            {
                Events.push_back(EventType { std::forward<Args>(Arguments)... });
            }
            else
            {
                Events.emplace_back(std::forward<Args>(Arguments)...);
            }
        }

        constexpr void Clear() noexcept override
        {
            Events.clear();
        }

        [[nodiscard]] constexpr SinkType GetSink() noexcept
        {
            return SinkType { Signal };
        }

        [[nodiscard]] constexpr std::size_t GetSize() const noexcept override
        {
            return Events.size();
        }

    private:
        SignalType Signal;
        ContainerType Events;
    };
}

#endif // ENGINE_SOURCES_EVENTS_FILE_EVENT_LOOP_H
