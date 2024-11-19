#ifndef ENGINE_SOURCES_EVENTS_TYPES_WINDOW_EVENT_FILE_WINDOW_EVENT_H
#define ENGINE_SOURCES_EVENTS_TYPES_WINDOW_EVENT_FILE_WINDOW_EVENT_H

#include "../../../GUI/Window/Window.h"

#include <memory>

namespace egg::Events
{
    class WindowEvent
    {
    public:
        explicit WindowEvent(const std::shared_ptr<GUI::Window>& Window);

        [[nodiscard]] std::shared_ptr<GUI::Window> GetWindow() const;

    private:
        std::shared_ptr<GUI::Window> Window;
    };
}

#endif // ENGINE_SOURCES_EVENTS_TYPES_WINDOW_EVENT_FILE_WINDOW_EVENT_H
