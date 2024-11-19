#include "./WindowCreated.h"

namespace egg::Events
{
    WindowCreated::WindowCreated(const std::shared_ptr<GUI::Window>& Window) : WindowEvent { Window }
    {
    }
}
