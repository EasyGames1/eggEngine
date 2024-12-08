#ifndef ENGINE_SOURCES_TYPES_TRAITS_INTERNAL_FILE_IS_ALL_SAME_H
#define ENGINE_SOURCES_TYPES_TRAITS_INTERNAL_FILE_IS_ALL_SAME_H

#include <type_traits>

namespace egg::Types::Internal
{
    template <typename...>
    struct IsAllSame;

    template <typename Type, typename... Types>
    struct IsAllSame<Type, Type, Types...> : IsAllSame<Type, Types...>
    {
    };

    template <typename Type, typename Other, typename... Types>
    struct IsAllSame<Type, Other, Types...> : std::false_type
    {
    };

    template <typename... Types> requires (sizeof...(Types) <= 1u)
    struct IsAllSame<Types...> : std::true_type
    {
    };
}

#endif // ENGINE_SOURCES_TYPES_TRAITS_INTERNAL_FILE_IS_ALL_SAME_H
