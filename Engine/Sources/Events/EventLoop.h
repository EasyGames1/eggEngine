#ifndef ENGINE_SOURCES_EVENTS_FILE_EVENT_LOOP_H
#define ENGINE_SOURCES_EVENTS_FILE_EVENT_LOOP_H

#include "Signal.h"
#include "Sink.h"
#include "Type/Traits/Capabilities.h"

#include <concepts>
#include <memory>
#include <vector>

namespace egg::Events
{
    template <typename Type, Types::ValidAllocator<Type> AllocatorParameter> requires std::same_as<Type, std::decay_t<Type>>
    class EventLoop final
    {
        using LoopAllocatorTraits = std::allocator_traits<AllocatorParameter>;

        using CallbackSignature = void(Type&);
        using SignalType = Signal<CallbackSignature, typename LoopAllocatorTraits::template rebind_alloc<Delegate<CallbackSignature>>>;

        using ContainerType = std::vector<Type, AllocatorParameter>;

    public:
        using AllocatorType = AllocatorParameter;

        using SinkType = Sink<SignalType>;


        explicit EventLoop(const AllocatorType& Allocator) : Signal { Allocator }, Events { Allocator }
        {
        }

        void Publish()
        {
            for (auto& Event : Events)
            {
                Signal.Publish(Event);
            }
            Clear();
        }

        void Trigger(Type Event)
        {
            Signal.Publish(Event);
        }

        template <typename... Args>
        void Enqueue(Args&&... Arguments)
        {
            if constexpr (std::is_aggregate_v<Type> && (sizeof...(Args) != 0u || !std::default_initializable<Type>))
            {
                Events.push_back(Type { std::forward<Args>(Arguments)... });
            }
            else
            {
                Events.emplace_back(std::forward<Args>(Arguments)...);
            }
        }

        void Clear() noexcept
        {
            Events.clear();
        }

        [[nodiscard]] SinkType GetSink() noexcept
        {
            return SinkType { Signal };
        }

        [[nodiscard]] std::size_t GetSize() const noexcept
        {
            return Events.size();
        }

    private:
        SignalType Signal;
        ContainerType Events;
    };
}

#endif // ENGINE_SOURCES_EVENTS_FILE_EVENT_LOOP_H
