#ifndef ENGINE_SOURCES_EVENTS_TYPES_WINDOW_CREATED_FILE_WINDOW_CREATED_H
#define ENGINE_SOURCES_EVENTS_TYPES_WINDOW_CREATED_FILE_WINDOW_CREATED_H

#include <Events/Types/WindowEvent/WindowEvent.h>
#include <GUI/Window/Window.h>

namespace egg::Events
{
    class WindowCreated : public WindowEvent
    {
    public:
        explicit WindowCreated(const std::shared_ptr<GUI::Window>& Window);
    };
}

#endif // ENGINE_SOURCES_EVENTS_TYPES_WINDOW_CREATED_FILE_WINDOW_CREATED_H
