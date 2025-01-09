#ifndef ENGINE_SOURCES_TYPES_DEDUCTION_INTERNAL_FILE_TUPLE_SIZE_H
#define ENGINE_SOURCES_TYPES_DEDUCTION_INTERNAL_FILE_TUPLE_SIZE_H
#include <type_traits>

namespace egg::Types::Internal
{
    template <typename Tuple>
    struct TupleSize;

    template <template<typename...> typename Tuple, typename... Types>
    struct TupleSize<Tuple<Types...>> : std::integral_constant<std::size_t, sizeof...(Types)>
    {
    };
}

#endif // ENGINE_SOURCES_TYPES_DEDUCTION_INTERNAL_FILE_TUPLE_SIZE_H
