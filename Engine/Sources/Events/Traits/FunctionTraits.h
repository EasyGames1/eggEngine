#ifndef ENGINE_SOURCES_EVENTS_TRAITS_FILE_FUNCTION_TRAITS_H
#define ENGINE_SOURCES_EVENTS_TRAITS_FILE_FUNCTION_TRAITS_H

#include <utility>

namespace egg::Events
{
    template <typename ReturnType, typename... Args>
    struct FunctionTraits
    {
        using Type = ReturnType(Args...);
        using Pointer = Type*;
        using ResultType = ReturnType;
        using ArgumentsTuple = std::tuple<Args...>;

        template <typename... Before>
        using IndexSequenceFor = std::index_sequence_for<Before..., Args...>;
    };
}

#endif // ENGINE_SOURCES_EVENTS_TRAITS_FILE_FUNCTION_TRAITS_H
