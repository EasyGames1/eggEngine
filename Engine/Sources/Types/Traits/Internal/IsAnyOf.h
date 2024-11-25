#ifndef ENGINE_SOURCES_TYPES_TRAITS_INTERNAL_FILE_IS_ONE_OF_H
#define ENGINE_SOURCES_TYPES_TRAITS_INTERNAL_FILE_IS_ONE_OF_H

#include <type_traits>

namespace egg::Types::Internal
{
    template <typename, typename...>
    struct IsAnyOf;

    template <typename Type, typename Other, typename... Types>
    struct IsAnyOf<Type, Other, Types...> : IsAnyOf<Type, Types...>
    {
    };

    template <typename Type>
    struct IsAnyOf<Type> : std::false_type
    {
    };

    template <typename Type, typename... Types>
    struct IsAnyOf<Type, Type, Types...> : std::true_type
    {
    };
}

#endif // ENGINE_SOURCES_TYPES_TRAITS_INTERNAL_FILE_IS_ONE_OF_H
