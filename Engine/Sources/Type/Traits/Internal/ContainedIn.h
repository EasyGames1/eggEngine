#ifndef ENGINE_SOURCES_TYPE_TRAITS_INTERNAL_FILE_CONTAINED_IN_H
#define ENGINE_SOURCES_TYPE_TRAITS_INTERNAL_FILE_CONTAINED_IN_H

#include <tuple>

namespace egg::Types::Internal
{
    template <typename, typename>
    struct ContainedIn;

    template <typename Type, typename Other, typename... Types>
    struct ContainedIn<Type, std::tuple<Other, Types...>> : ContainedIn<Type, std::tuple<Types...>>
    {
    };

    template <typename Type>
    struct ContainedIn<Type, std::tuple<>> : std::false_type
    {
    };

    template <typename Type, typename... Types>
    struct ContainedIn<Type, std::tuple<Type, Types...>> : std::true_type
    {
    };
}


#endif // ENGINE_SOURCES_TYPE_TRAITS_INTERNAL_FILE_CONTAINED_IN_H
