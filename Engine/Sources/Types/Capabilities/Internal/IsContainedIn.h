#ifndef ENGINE_SOURCES_TYPES_CAPABILITIES_INTERNAL_FILE_IS_CONTAINED_IN_H
#define ENGINE_SOURCES_TYPES_CAPABILITIES_INTERNAL_FILE_IS_CONTAINED_IN_H

#include <type_traits>

namespace egg::Types::Internal
{
    template <typename, typename...>
    struct IsContainedIn;

    template <typename Type, typename Other, typename... Types>
    struct IsContainedIn<Type, Other, Types...> : IsContainedIn<Type, Types...>
    {
    };

    template <typename Type>
    struct IsContainedIn<Type> : std::false_type
    {
    };

    template <typename Type, typename... Types>
    struct IsContainedIn<Type, Type, Types...> : std::true_type
    {
    };


    template <typename, typename>
    struct IsContainedInTuple;

    template <typename Type, template<typename...> typename Tuple, typename... Types>
    struct IsContainedInTuple<Type, Tuple<Types...>> : IsContainedIn<Type, Types...>
    {
    };
}

#endif // ENGINE_SOURCES_TYPES_CAPABILITIES_INTERNAL_FILE_IS_CONTAINED_IN_H
