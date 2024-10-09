#ifndef ENGINE_SOURCES_ECS_TRAITS_FILE_COMPONENT_TRAITS_H
#define ENGINE_SOURCES_ECS_TRAITS_FILE_COMPONENT_TRAITS_H

#include "./PageSizeTraits.h"

#include <type_traits>

namespace egg::ECS
{
    template <typename ComponentType, typename = void> requires std::is_same_v<std::decay_t<ComponentType>, ComponentType>
    struct ComponentTraits
    {
        using Type = ComponentType;

        static constexpr std::size_t PageSize { PageSizeTraits<ComponentType>::value };
    };
}

#endif // ENGINE_SOURCES_ECS_TRAITS_FILE_COMPONENT_TRAITS_H
