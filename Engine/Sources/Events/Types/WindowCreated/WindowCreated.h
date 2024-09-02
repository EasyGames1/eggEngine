#ifndef ENGINE_SOURCES_EVENTS_TYPES_WINDOW_CREATED_FILE_WINDOW_CREATED_H
#define ENGINE_SOURCES_EVENTS_TYPES_WINDOW_CREATED_FILE_WINDOW_CREATED_H

#include "../../../GUI/Window/Window.h"
#include "../WindowEvent/WindowEvent.h"

namespace egg::Events
{
    class WindowCreated : public WindowEvent
    {
    public:
        explicit WindowCreated(GUI::Window& Window);
    };
}

#endif // ENGINE_SOURCES_EVENTS_TYPES_WINDOW_CREATED_FILE_WINDOW_CREATED_H
