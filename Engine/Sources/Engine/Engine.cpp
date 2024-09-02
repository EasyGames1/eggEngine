#include "Engine.h"

#include "../ECS/Systems/Renderer/Renderer.h"
#include "../Events/Types/WindowCreated/WindowCreated.h"
#include "../Scene/Scene.h"

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
        Scene::GetDispatcher().Subscribe<Events::WindowCreated>(
            [this](const Events::WindowCreated& Event)
            {
                Window = &Event.GetWindow();
            }
        );

        for (auto* System : Systems)
        {
            System->Initialize();
        }
    }

    void Engine::MainLoop()
    {
        while (!Window->IsShouldClose())
        {
            glfwPollEvents();

            for (auto* System : Systems)
            {
                System->Update(0);
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
