#ifndef ENGINE_SOURCES_ENGINE_FILE_ENGINE_H
#define ENGINE_SOURCES_ENGINE_FILE_ENGINE_H

#include "../ECS/Systems/System.h"
#include "../GUI/Window/Window.h"

#include <vector>

namespace egg
{
    class Engine
    {
    public:
        Engine();

        ~Engine();

        void Run();

        template <typename System, typename... Args>
        void AddSystem(Args&&... Arguments)
        {
            Systems.emplace_back(new System { std::forward<Args>(Arguments)... });
        }

    private:
        void Init();

        void MainLoop();

        void End();

        std::vector<Systems::System*> Systems;

        GUI::Window* Window {};
    };
}

#endif // ENGINE_SOURCES_ENGINE_FILE_ENGINE_H
