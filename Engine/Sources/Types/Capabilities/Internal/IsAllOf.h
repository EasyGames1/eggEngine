#ifndef ENGINE_SOURCES_TYPES_CAPABILITIES_INTERNAL_FILE_IS_ALL_OF_H
#define ENGINE_SOURCES_TYPES_CAPABILITIES_INTERNAL_FILE_IS_ALL_OF_H

#include "./IsPredicateTrait.h"

#include <type_traits>

namespace egg::Types::Internal
{
    template <template<typename> typename PredicateType, typename... Types>
        requires (IsPredicateTrait<PredicateType<Types>>::value && ...)
    struct IsAllOf : std::conjunction<PredicateType<Types>...>
    {
    };


    template <template<typename> typename, typename>
    struct IsAllOfTuple;

    template <template<typename> typename PredicateType, template<typename...> typename Tuple, typename... Types>
        requires (IsPredicateTrait<PredicateType<Types>>::value && ...)
    struct IsAllOfTuple<PredicateType, Tuple<Types...>> : IsAllOf<PredicateType, Types...>
    {
    };
}

#endif // ENGINE_SOURCES_TYPES_CAPABILITIES_INTERNAL_FILE_IS_ALL_OF_H
