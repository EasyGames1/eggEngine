#ifndef ENGINE_SOURCES_ECS_SYSTEMS_FILE_SYSTEM_H
#define ENGINE_SOURCES_ECS_SYSTEMS_FILE_SYSTEM_H

namespace egg::Systems
{
    class System
    {
    public:
        virtual ~System() = default;

        virtual void Initialize() = 0;

        virtual void Update(float DeltaTime) = 0;

        virtual void Terminate() = 0;
    };
}

#endif // ENGINE_SOURCES_ECS_SYSTEMS_FILE_SYSTEM_H
