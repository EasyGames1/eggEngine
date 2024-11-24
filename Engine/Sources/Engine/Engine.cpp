#include "./Engine.h"

#include <ECS/Systems/Renderer/Renderer.h>
#include <Events/Types/WindowCreated/WindowCreated.h>
#include <Scene/Scene.h>

namespace egg
{
    Engine::Engine() : Systems { new Systems::Renderer {} }
    {
    }

    Engine::~Engine()
    {
        for (const auto* System : Systems)
        {
            delete System;
        }
    }

    void Engine::Run()
    {
        Init();
        MainLoop();
        End();
    }

    void Engine::Init()
    {
        Scene::GetDispatcher().GetSink<Events::WindowCreated>().Connect<&Engine::InitWindow>(*this);

        for (auto* System : Systems)
        {
            System->Initialize();
        }
    }

    void Engine::InitWindow(const Events::WindowCreated& Event)
    {
        Window = Event.GetWindow();
    }

    void Engine::MainLoop()
    {
        while (!Window->GetShouldClose())
        {
            glfwPollEvents();

            for (auto* System : Systems)
            {
                System->Update(0u);
            }
        }
    }

    void Engine::End()
    {
        for (auto* System : Systems)
        {
            System->Terminate();
        }
    }
}
