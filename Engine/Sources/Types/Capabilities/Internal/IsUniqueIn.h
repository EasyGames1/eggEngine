#ifndef ENGINE_SOURCES_TYPES_CAPABILITIES_INTERNAL_FILE_IS_UNIQUE_IN_H
#define ENGINE_SOURCES_TYPES_CAPABILITIES_INTERNAL_FILE_IS_UNIQUE_IN_H

#include <type_traits>

namespace egg::Types::Internal
{
    template <typename, typename...>
    struct IsUniqueIn;

    template <typename Type, typename Other, typename... Types>
    struct IsUniqueIn<Type, Other, Types...> : IsUniqueIn<Type, Types...>
    {
    };

    template <typename Type, typename... Types>
    struct IsUniqueIn<Type, Type, Types...> : std::false_type
    {
    };

    template <typename Type>
    struct IsUniqueIn<Type> : std::true_type
    {
    };
}

#endif // ENGINE_SOURCES_TYPES_CAPABILITIES_INTERNAL_FILE_IS_UNIQUE_IN_H
