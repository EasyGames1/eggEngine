#ifndef ENGINE_SOURCES_TYPES_DEDUCTION_INTERNAL_FILE_FILTER_TUPLE_H
#define ENGINE_SOURCES_TYPES_DEDUCTION_INTERNAL_FILE_FILTER_TUPLE_H

#include "./CombineTuples.h"

#include <Types/Capabilities/Capabilities.h>

namespace egg::Types::Internal
{
    template <template<typename...> typename, typename>
    struct FilterTuple;

    template <template<typename...> typename PredicateType, template<typename...> typename Tuple, typename... Types>
        requires (Predicate<PredicateType<Types>> && ...)
    struct FilterTuple<PredicateType, Tuple<Types...>>
        : CombineTuples<Tuple, std::conditional_t<PredicateType<Types>::value, Tuple<Types>, Tuple<>>...>
    {
    };
}

#endif // ENGINE_SOURCES_TYPES_DEDUCTION_INTERNAL_FILE_FILTER_TUPLE_H
