#ifndef ENGINE_SOURCES_ENGINE_FILE_ENGINE_H
#define ENGINE_SOURCES_ENGINE_FILE_ENGINE_H

#include "../ECS/Systems/System.h"
#include "../GUI/Window/Window.h"
#include "Events/Types/WindowCreated/WindowCreated.h"

#include <memory>
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

        void InitWindow(const Events::WindowCreated& Event);

        void MainLoop();

        void End();

        std::vector<Systems::System*> Systems;

        std::shared_ptr<GUI::Window> Window {};
    };
}

#endif // ENGINE_SOURCES_ENGINE_FILE_ENGINE_H
