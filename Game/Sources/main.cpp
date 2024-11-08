#include "Engine/Engine.h"
#include "Events/Signal.h"

#include <cstdlib>

int main(const int argc, char** argv)
{
    egg::Events::Signal<void(), std::allocator<egg::Events::Delegate<void()>>> v;
    // egg::Events::Connection fetch { v };
    egg::Engine Engine;
    Engine.Run();

    egg::Events::Signal<void()> s;

    return EXIT_SUCCESS;
}
