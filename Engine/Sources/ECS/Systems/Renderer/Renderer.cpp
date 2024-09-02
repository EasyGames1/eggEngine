#include "Renderer.h"

#include "../../../Constants/Window.h"
#include "../../../Events/Types/WindowCreated/WindowCreated.h"
#include "../../../GUI/Window/Window.h"
#include "../../../Scene/Scene.h"

#include <stdexcept>

namespace egg
{
    void Systems::Renderer::Initialize()
    {
        InitWindow();
    }

    void Systems::Renderer::Update(float DeltaTime)
    {
    }

    void Systems::Renderer::Terminate()
    {
    }

    Systems::Renderer::~Renderer()
    {
        delete Window;
        glfwTerminate();
    }

    void Systems::Renderer::InitWindow()
    {
        if (!glfwInit()) throw std::runtime_error("Failed to initialize glfw");

        Window = new GUI::Window(
            Constants::Window::Name.data(),
            Constants::Window::InitialWidth,
            Constants::Window::InitialHeight,
            GUI::Window::ClientAPI::None
        );

        Scene::GetDispatcher().Notify(Events::WindowCreated { *Window });
    }
}
