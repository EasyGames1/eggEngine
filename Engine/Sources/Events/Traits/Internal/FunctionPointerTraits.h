#ifndef ENGINE_SOURCES_EVENTS_TRAITS_INTERNAL_FILE_FUNCTION_POINTER_TRAITS_H
#define ENGINE_SOURCES_EVENTS_TRAITS_INTERNAL_FILE_FUNCTION_POINTER_TRAITS_H

#include "../FunctionTraits.h"

#include <tuple>

namespace egg::Events::Internal
{
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
}


#endif // ENGINE_SOURCES_EVENTS_TRAITS_INTERNAL_FILE_FUNCTION_POINTER_TRAITS_H
