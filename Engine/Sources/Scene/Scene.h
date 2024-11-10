#ifndef ENGINE_SOURCES_SCENE_FILE_SCENE_H
#define ENGINE_SOURCES_SCENE_FILE_SCENE_H

#include "../Events/Dispatcher_Deprecated.h"

namespace egg
{
    class Scene
    {
    public:
        Scene(const Scene&) = delete;

        const Scene& operator=(const Scene&) = delete;

        [[nodiscard]] static Events::Dispatcher_Deprecated& GetDispatcher();

    private:
        Scene() = default;

        static Scene& Get();

        Events::Dispatcher_Deprecated Dispatcher;
    };
}

#endif // ENGINE_SOURCES_SCENE_FILE_SCENE_H
