#ifndef ENGINE_SOURCES_SCENE_FILE_SCENE_H
#define ENGINE_SOURCES_SCENE_FILE_SCENE_H

#include "../Events/Dispatcher.h"

namespace egg
{
    class Scene
    {
    public:
        Scene(const Scene&) = delete;

        const Scene& operator=(const Scene&) = delete;

        [[nodiscard]] static Events::Dispatcher& GetDispatcher();

    private:
        Scene() = default;

        static Scene& Get();

        Events::Dispatcher Dispatcher;
    };
}

#endif // ENGINE_SOURCES_SCENE_FILE_SCENE_H
