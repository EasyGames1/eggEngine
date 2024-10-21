#ifndef ENGINE_SOURCES_EVENTS_TRAITS_FILE_FUNCTION_POINTER_TRAITS_H
#define ENGINE_SOURCES_EVENTS_TRAITS_FILE_FUNCTION_POINTER_TRAITS_H

#include <tuple>

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


    template <typename, typename...>
    struct FunctionPointerTraits;


    template <typename ReturnType, typename... Args>
    struct FunctionPointerTraits<ReturnType(*)(Args...)> : FunctionTraits<ReturnType, Args...>
    {
    };


    template <typename ReturnType, typename... Args>
    struct FunctionPointerTraits<ReturnType(*)(Args...) noexcept> : FunctionTraits<ReturnType, Args...>
    {
    };


    template <typename ReturnType, typename Type, typename... Args, typename Other>
    struct FunctionPointerTraits<ReturnType(*)(Type, Args...), Other> : FunctionTraits<ReturnType, Args...>
    {
        using InstanceType = Type;
    };


    template <typename ReturnType, typename Type, typename... Args, typename Other>
    struct FunctionPointerTraits<ReturnType(*)(Type, Args...) noexcept, Other> : FunctionTraits<ReturnType, Args...>
    {
        using InstanceType = Type;
    };


    template <typename Class, typename ReturnType, typename... Args, typename... Other>
    struct FunctionPointerTraits<ReturnType(Class::*)(Args...), Other...> : FunctionTraits<ReturnType, Args...>
    {
        using InstanceType = Class;
    };


    template <typename Class, typename ReturnType, typename... Args, typename... Other>
    struct FunctionPointerTraits<ReturnType(Class::*)(Args...) noexcept, Other...> : FunctionTraits<ReturnType, Args...>
    {
        using InstanceType = Class;
    };


    template <typename Class, typename ReturnType, typename... Args, typename... Other>
    struct FunctionPointerTraits<ReturnType(Class::*)(Args...) const, Other...> : FunctionTraits<ReturnType, Args...>
    {
        using InstanceType = Class;
    };


    template <typename Class, typename ReturnType, typename... Args, typename... Other>
    struct FunctionPointerTraits<ReturnType(Class::*)(Args...) const noexcept, Other...> : FunctionTraits<ReturnType, Args...>
    {
        using InstanceType = Class;
    };


    template <typename Class, typename Type, typename... Other>
    struct FunctionPointerTraits<Type Class::*, Other...> : FunctionTraits<Type>
    {
        using InstanceType = Class;
    };


    template <typename Type>
    concept FreeFunctionPointer = !requires { typename FunctionPointerTraits<Type>::InstanceType; };

    template <typename Type, typename FunctionPointerType>
    concept ValidInstance =
        std::is_same_v<std::remove_cvref_t<std::remove_pointer_t<Type>>, typename FunctionPointerTraits<FunctionPointerType>::InstanceType>;

    template <typename Type, typename FunctionPointerType>
    concept ValidValueOrInstance = FreeFunctionPointer<FunctionPointerType> || ValidInstance<Type, FunctionPointerType>;
}

#endif // ENGINE_SOURCES_EVENTS_TRAITS_FILE_FUNCTION_POINTER_TRAITS_H
