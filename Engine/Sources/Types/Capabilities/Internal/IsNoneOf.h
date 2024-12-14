#ifndef ENGINE_SOURCES_TYPES_CAPABILITIES_INTERNAL_FILE_IS_NONE_OF_H
#define ENGINE_SOURCES_TYPES_CAPABILITIES_INTERNAL_FILE_IS_NONE_OF_H

#include "./IsPredicateTrait.h"

namespace egg::Types::Internal
{
    template <template<typename> typename PredicateType, typename... Types>
        requires (IsPredicateTrait<PredicateType<Types>>::value && ...)
    struct IsNoneOf : std::conjunction<std::negation<PredicateType<Types>>...>
    {
    };


    template <template<typename> typename, typename>
    struct IsNoneOfTuple;

    template <template<typename> typename PredicateType, template<typename...> typename Tuple, typename... Types>
        requires (IsPredicateTrait<PredicateType<Types>>::value && ...)
    struct IsNoneOfTuple<PredicateType, Tuple<Types...>> : IsNoneOf<PredicateType, Types...>
    {
    };
}

#endif // ENGINE_SOURCES_TYPES_CAPABILITIES_INTERNAL_FILE_IS_NONE_OF_H
