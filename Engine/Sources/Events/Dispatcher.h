#ifndef ENGINE_SOURCES_EVENTS_FILE_DISPATCHER_H
#define ENGINE_SOURCES_EVENTS_FILE_DISPATCHER_H

#include <functional>

namespace egg::Events
{
    ///@deprecated
    class Dispatcher
    {
    public:
        template <typename EventType>
        using HandlerType = std::function<void(const EventType&)>;

        template <typename EventType>
        std::size_t Subscribe(HandlerType<EventType> Handler)
        {
            auto& Handlers { GetHandlers<EventType>() };
            Handlers.push_back(Handler);
            return Handlers.size() - 1u;
        }

        template <typename EventType>
        void Notify(const EventType& Event)
        {
            for (const auto& Handler : GetHandlers<EventType>())
            {
                Handler(Event);
            }
        }

        template <typename EventType>
        void Unsubscribe(std::size_t Index)
        {
            //TODO: ?
            auto& Handlers { GetHandlers<EventType>() };
        }

    private:
        template <typename EventType>
        using ContainerType = std::vector<HandlerType<EventType>>;

        template <typename EventType>
        ContainerType<EventType>& GetHandlers()
        {
            static ContainerType<EventType> EventHandlers;
            return EventHandlers;
        }
    };
}

#endif // ENGINE_SOURCES_EVENTS_FILE_DISPATCHER_H
