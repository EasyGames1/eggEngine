#include "./Scene.h"

namespace egg
{
    Events::Dispatcher_Deprecated& Scene::GetDispatcher()
    {
        return Get().Dispatcher;
    }

    Scene& Scene::Get()
    {
        static Scene CurrentScene;
        return CurrentScene;
    }
} // egg
