#ifndef ENGINE_SOURCES_TYPES_TRAITS_INTERNAL_FILE_INSTANCE_OF_H
#define ENGINE_SOURCES_TYPES_TRAITS_INTERNAL_FILE_INSTANCE_OF_H

#include <type_traits>

namespace egg::Types::Internal
{
    template <typename, template<typename...>typename>
    struct IsInstanceOf : std::false_type
    {
    };

    template <template<typename...>typename Template, typename... Args>
    struct IsInstanceOf<Template<Args...>, Template> : std::true_type
    {
    };
}

#endif // ENGINE_SOURCES_TYPES_TRAITS_INTERNAL_FILE_INSTANCE_OF_H
