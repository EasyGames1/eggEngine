#ifndef ENGINE_SOURCES_TYPES_DEDUCTION_INTERNAL_FILE_TYPE_INDEX_IN_H
#define ENGINE_SOURCES_TYPES_DEDUCTION_INTERNAL_FILE_TYPE_INDEX_IN_H

#include <tuple>
#include <type_traits>

namespace egg::Types::Internal
{
    template <typename, typename>
    struct TypeIndexIn;

    template <typename Type, template<typename...> typename Tuple, typename... Types>
    struct TypeIndexIn<Type, Tuple<Type, Types...>> : std::integral_constant<std::size_t, 0u>
    {
    };

    template <typename Type, template<typename...> typename Tuple, typename Other, typename... Types>
    struct TypeIndexIn<Type, Tuple<Other, Types...>> : std::integral_constant<std::size_t, 1u + TypeIndexIn<Type, Tuple<Types...>>::value>
    {
    };
}

#endif // ENGINE_SOURCES_TYPES_DEDUCTION_INTERNAL_FILE_TYPE_INDEX_IN_H
