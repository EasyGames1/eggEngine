#ifndef ENGINE_SOURCES_TYPES_DEDUCTION_INTERNAL_FILE_TYPE_INDEX_IN_H
#define ENGINE_SOURCES_TYPES_DEDUCTION_INTERNAL_FILE_TYPE_INDEX_IN_H

#include <type_traits>

namespace egg::Types::Internal
{
    template <typename, typename...>
    struct TypeIndexIn;

    template <typename Type, typename... Types>
    struct TypeIndexIn<Type, Type, Types...> : std::integral_constant<std::size_t, 0u>
    {
    };

    template <typename Type, typename Other, typename... Types>
    struct TypeIndexIn<Type, Other, Types...> : std::integral_constant<std::size_t, 1u + TypeIndexIn<Type, Types...>::value>
    {
    };


    template <typename, typename>
    struct TypeIndexInTuple;

    template <typename Type, template<typename...> typename Tuple, typename... Types>
    struct TypeIndexInTuple<Type, Tuple<Types...>> : TypeIndexIn<Type, Types...>
    {
    };
}

#endif // ENGINE_SOURCES_TYPES_DEDUCTION_INTERNAL_FILE_TYPE_INDEX_IN_H
