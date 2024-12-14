#ifndef ENGINE_SOURCES_TYPES_CAPABILITIES_INTERNAL_FILE_IS_ALL_UNIQUE_H
#define ENGINE_SOURCES_TYPES_CAPABILITIES_INTERNAL_FILE_IS_ALL_UNIQUE_H

#include "./IsUniqueIn.h"

namespace egg::Types::Internal
{
    template <typename...>
    struct IsAllUnique;

    template <typename Type, typename... Types> requires IsUniqueIn<Type, Types...>::value
    struct IsAllUnique<Type, Types...> : IsAllUnique<Types...>
    {
    };

    template <typename Type, typename... Types>
    struct IsAllUnique<Type, Types...> : std::false_type
    {
    };

    template <>
    struct IsAllUnique<> : std::true_type
    {
    };


    template <typename>
    struct IsAllTupleUnique;

    template <template<typename...> typename Tuple, typename... Types>
    struct IsAllTupleUnique<Tuple<Types...>> : IsAllUnique<Types...>
    {
    };
}

#endif // ENGINE_SOURCES_TYPES_CAPABILITIES_INTERNAL_FILE_IS_ALL_UNIQUE_H
