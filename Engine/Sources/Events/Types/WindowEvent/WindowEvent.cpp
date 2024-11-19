#include "./WindowEvent.h"

namespace egg::Events
{
    WindowEvent::WindowEvent(const std::shared_ptr<GUI::Window>& Window) : Window { Window }
    {
    }

    std::shared_ptr<GUI::Window> WindowEvent::GetWindow() const
    {
        return Window;
    }
}
