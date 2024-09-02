#ifndef ENGINE_SOURCES_EVENTS_TYPES_WINDOW_EVENT_FILE_WINDOW_EVENT_H
#define ENGINE_SOURCES_EVENTS_TYPES_WINDOW_EVENT_FILE_WINDOW_EVENT_H

#include "../../../GUI/Window/Window.h"

namespace egg::Events
{
    class WindowEvent
    {
    public:
        explicit WindowEvent(GUI::Window& Window);

        [[nodiscard]] GUI::Window& GetWindow() const;

    private:
        GUI::Window& Window;
    };
}

#endif // ENGINE_SOURCES_EVENTS_TYPES_WINDOW_EVENT_FILE_WINDOW_EVENT_H
