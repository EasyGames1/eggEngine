#ifndef ENGINE_SOURCES_ECS_TRAITS_FILE_COMPONENT_TRAITS_H
#define ENGINE_SOURCES_ECS_TRAITS_FILE_COMPONENT_TRAITS_H

#include "PageSizeTraits.h"

#include <type_traits>

namespace egg::ECS
{
    template <typename ComponentType, typename = void>
    struct ComponentTraits
    {
        static_assert(std::is_same_v<std::decay_t<ComponentType>, ComponentType>, "Unsupported type");

        using Type = ComponentType;

        static constexpr std::size_t PageSize { PageSizeTraits<ComponentType>::value };
    };

    template <typename Type>
    concept ZeroSizedComponent = !sizeof(Type);
}

#endif // ENGINE_SOURCES_ECS_TRAITS_FILE_COMPONENT_TRAITS_H
