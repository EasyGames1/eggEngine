#ifndef ENGINE_SOURCES_EVENTS_TRAITS_FILE_FUNCTION_POINTER_TRAITS_H
#define ENGINE_SOURCES_EVENTS_TRAITS_FILE_FUNCTION_POINTER_TRAITS_H

namespace egg::Events
{
    template <typename ReturnType, typename... Args>
    struct BasicFunctionPointerTraits
    {
        using Type = ReturnType(Args...);
        using Pointer = Type*;
        using ResultType = ReturnType;
        using ArgumentTypes = std::tuple<Args...>;

        template <typename... Before>
        using IndexSequenceFor = std::index_sequence_for<Before..., Args...>;
    };

    template <typename>
    struct FunctionPointerTraits;

    template <typename ReturnType, typename... Args>
    struct FunctionPointerTraits<ReturnType(*)(Args...)> : BasicFunctionPointerTraits<ReturnType, Args...>
    {
    };

    template <typename ReturnType, typename Type, typename... Args>
    struct FunctionPointerTraits<ReturnType(*)(Type, Args...)> : BasicFunctionPointerTraits<ReturnType, Args...>
    {
        using InstanceType = Type;
    };

    template <typename Class, typename ReturnType, typename... Args>
    struct FunctionPointerTraits<ReturnType(Class::*)(Args...)> : BasicFunctionPointerTraits<ReturnType, Args...>
    {
        using InstanceType = Class;
    };

    template <typename Class, typename ReturnType, typename... Args>
    struct FunctionPointerTraits<ReturnType(Class::*)(Args...) const> : BasicFunctionPointerTraits<ReturnType, Args...>
    {
        using InstanceType = Class;
    };

    template <typename Class, typename Type>
    struct FunctionPointerTraits<Type Class::*> : BasicFunctionPointerTraits<Type>
    {
        using InstanceType = Class;
    };
}

#endif // ENGINE_SOURCES_EVENTS_TRAITS_FILE_FUNCTION_POINTER_TRAITS_H
