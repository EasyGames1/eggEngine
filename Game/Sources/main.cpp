#include "Engine/Engine.h"
#include "Events/Signal.h"

#include <cstdlib>

int main(const int argc, char** argv)
{
    egg::Engine Engine;
    Engine.Run();

    return EXIT_SUCCESS;
}
