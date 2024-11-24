#ifndef ENGINE_SOURCES_ECS_TRAITS_FILE_COMPONENT_TRAITS_H
#define ENGINE_SOURCES_ECS_TRAITS_FILE_COMPONENT_TRAITS_H

#include <ECS/Traits/PageSizeTraits.h>
#include <Types/Traits/Capabilities.h>

namespace egg::ECS
{
    template <Types::Decayed ComponentType>
    struct ComponentTraits
    {
        using Type = ComponentType;

        static constexpr std::size_t PageSize { PageSizeTraits<ComponentType>::value };
    };
}

#endif // ENGINE_SOURCES_ECS_TRAITS_FILE_COMPONENT_TRAITS_H
