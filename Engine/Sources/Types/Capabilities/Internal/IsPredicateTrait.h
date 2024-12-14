#ifndef ENGINE_SOURCES_TYPES_CAPABILITIES_INTERNAL_FILE_IS_PREDICATE_TRAIT_H
#define ENGINE_SOURCES_TYPES_CAPABILITIES_INTERNAL_FILE_IS_PREDICATE_TRAIT_H

#include <concepts>
#include <type_traits>

namespace egg::Types::Internal
{
    template <typename Type>
    using IsPredicateTrait = std::bool_constant<std::convertible_to<decltype(Type::value), bool>>;
}

#endif // ENGINE_SOURCES_TYPES_CAPABILITIES_INTERNAL_FILE_IS_PREDICATE_TRAIT_H
