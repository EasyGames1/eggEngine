#ifndef ENGINE_SOURCES_EVENTS_FILE_EVENT_LOOP_INTERFACE_H
#define ENGINE_SOURCES_EVENTS_FILE_EVENT_LOOP_INTERFACE_H

#include <cstddef>

namespace egg::Events
{
    struct EventLoopInterface
    {
        constexpr virtual ~EventLoopInterface() = default;
        constexpr virtual void Publish() = 0;
        constexpr virtual void Clear() noexcept = 0;
        [[nodiscard]] constexpr virtual std::size_t GetSize() const noexcept = 0;
    };
}

#endif // ENGINE_SOURCES_EVENTS_FILE_EVENT_LOOP_INTERFACE_H
