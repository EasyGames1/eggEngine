#ifndef ENGINE_SOURCES_TYPES_TRAITS_INTERNAL_FILE_IS_ANY_OF_H
#define ENGINE_SOURCES_TYPES_TRAITS_INTERNAL_FILE_IS_ANY_OF_H

#include "./IsPredicateTrait.h"

#include <type_traits>

namespace egg::Types::Internal
{
    template <template<typename> typename PredicateType, typename... Types>
        requires (IsPredicateTrait<PredicateType<Types>>::value && ...)
    struct IsAnyOf : std::disjunction<PredicateType<Types>...>
    {
    };


    template <template<typename> typename, typename>
    struct IsAnyOfTuple;

    template <template<typename> typename PredicateType, template<typename...> typename Tuple, typename... Types>
        requires (IsPredicateTrait<PredicateType<Types>>::value && ...)
    struct IsAnyOfTuple<PredicateType, Tuple<Types...>> : IsAnyOf<PredicateType, Types...>
    {
    };
}

#endif // ENGINE_SOURCES_TYPES_TRAITS_INTERNAL_FILE_IS_ANY_OF_H
