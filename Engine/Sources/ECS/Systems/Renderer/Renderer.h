#ifndef ENGINE_SOURCES_ECS_SYSTEMS_RENDERER_FILE_RENDERER_H
#define ENGINE_SOURCES_ECS_SYSTEMS_RENDERER_FILE_RENDERER_H

#include "../System.h"
#include "../../../GUI/Window/Window.h"

#include <memory>

namespace egg::Systems
{
    class Renderer final : public System
    {
    public:
        void Initialize() override;

        void Update(float DeltaTime) override;

        void Terminate() override;

        ~Renderer() override;

    private:
        void InitWindow();

        std::shared_ptr<GUI::Window> Window {};
    };
}

#endif // ENGINE_SOURCES_ECS_SYSTEMS_RENDERER_FILE_RENDERER_H
