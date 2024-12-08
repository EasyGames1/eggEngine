#ifndef ENGINE_SOURCES_TYPES_CAPABILITIES_INTERNAL_FILE_IS_APPLICABLE_H
#define ENGINE_SOURCES_TYPES_CAPABILITIES_INTERNAL_FILE_IS_APPLICABLE_H

#include <array>
#include <tuple>

namespace egg::Types::Internal
{
    template <typename, typename, typename>
    struct IsApplicableHelper;

    template <typename Callable, typename TupleLike, std::size_t... Indices>
    struct IsApplicableHelper<Callable, TupleLike, std::index_sequence<Indices...>>
        : std::is_invocable<Callable, decltype(std::get<Indices>(std::declval<TupleLike>()))...>
    {
    };

    template <typename Callable, typename TupleLike>
    struct IsApplicable
        : IsApplicableHelper<Callable, TupleLike, std::make_index_sequence<std::tuple_size_v<std::decay_t<TupleLike>>>>
    {
    };
}

#endif // ENGINE_SOURCES_TYPES_CAPABILITIES_INTERNAL_FILE_IS_APPLICABLE_H
