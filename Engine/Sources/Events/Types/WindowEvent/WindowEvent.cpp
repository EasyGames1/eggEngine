#include "./WindowEvent.h"

namespace egg::Events
{
    WindowEvent::WindowEvent(GUI::Window& Window) : Window { Window }
    {
    }

    GUI::Window& WindowEvent::GetWindow() const
    {
        return Window;
    }
}
