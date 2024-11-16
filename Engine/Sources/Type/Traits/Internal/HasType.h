#ifndef ENGINE_SOURCES_TYPE_TRAITS_INTERNAL_FILE_HAS_TYPE_H
#define ENGINE_SOURCES_TYPE_TRAITS_INTERNAL_FILE_HAS_TYPE_H

#include <tuple>

namespace egg::Types::Internal
{
    template <typename, typename>
    struct HasType;

    template <typename Type, typename Other, typename... Types>
    struct HasType<Type, std::tuple<Other, Types...>> : HasType<Type, std::tuple<Types...>>
    {
    };

    template <typename Type>
    struct HasType<Type, std::tuple<>> : std::false_type
    {
    };

    template <typename Type, typename... Types>
    struct HasType<Type, std::tuple<Type, Types...>> : std::true_type
    {
    };
}


#endif // ENGINE_SOURCES_TYPE_TRAITS_INTERNAL_FILE_HAS_TYPE_H
