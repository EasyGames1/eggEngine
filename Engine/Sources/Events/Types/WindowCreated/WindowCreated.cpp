#include "WindowCreated.h"

namespace egg::Events
{
    WindowCreated::WindowCreated(GUI::Window& Window) : WindowEvent { Window }
    {
    }
}
