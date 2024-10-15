#include "Engine/Engine.h"
#include "Events/Signal.h"

#include <cstdlib>

int main(const int argc, char** argv)
{
    egg::Engine Engine;
    Engine.Run();

    egg::Events::Signal<void()> s;

    return EXIT_SUCCESS;
}
