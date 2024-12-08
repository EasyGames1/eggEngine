#ifndef ENGINE_SOURCES_TYPES_DEDUCTION_INTERNAL_FILE_REMOVE_TUPLE_INDEX_H
#define ENGINE_SOURCES_TYPES_DEDUCTION_INTERNAL_FILE_REMOVE_TUPLE_INDEX_H

#include "./CombineTuples.h"

#include <cstddef>
#include <type_traits>

namespace egg::Types::Internal
{
    template <std::size_t, typename>
    struct RemoveTupleIndex;

    template <template<typename...> typename Tuple, typename Type, typename... Types>
    struct RemoveTupleIndex<0u, Tuple<Type, Types...>> : std::type_identity<Tuple<Types...>>
    {
    };

    template <std::size_t Index, template<typename...> typename Tuple, typename Type, typename... Types>
    struct RemoveTupleIndex<Index, Tuple<Type, Types...>>
        : CombineTuples<Tuple, Tuple<Type>, typename RemoveTupleIndex<Index - 1u, Tuple<Types...>>::type>
    {
    };
}

#endif // ENGINE_SOURCES_TYPES_DEDUCTION_INTERNAL_FILE_REMOVE_TUPLE_INDEX_H
