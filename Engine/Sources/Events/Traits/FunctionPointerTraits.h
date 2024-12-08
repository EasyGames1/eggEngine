#ifndef ENGINE_SOURCES_EVENTS_TRAITS_FILE_FUNCTION_POINTER_TRAITS_H
#define ENGINE_SOURCES_EVENTS_TRAITS_FILE_FUNCTION_POINTER_TRAITS_H

#include "./Internal/FunctionPointerTraits.h"

#include <Types/Capabilities/Capabilities.h>
#include <Types/Deduction/Deduction.h>

#include <type_traits>

namespace egg::Events
{
    template <typename FunctionPointer, typename... Type>
    using FunctionPointerTraits = Internal::FunctionPointerTraits<std::remove_cv_t<FunctionPointer>, Type...>;

    template <typename Type>
    concept FreeFunctionPointer = !requires { typename FunctionPointerTraits<Type>::InstanceType; };

    template <typename Type, typename FunctionPointerType>
    concept ValidValue =
        FreeFunctionPointer<FunctionPointerType> &&
        std::convertible_to<Type, typename FunctionPointerTraits<FunctionPointerType, Type>::InstanceType>;

    template <typename Type, typename FunctionPointerType>
    concept ValidInstance = Types::Applicable<
        FunctionPointerType,
        Types::CombineTuples<
            std::tuple,
            std::tuple<Type>,
            typename FunctionPointerTraits<FunctionPointerType>::ArgumentsTuple
        >
    >;

    template <typename Type, typename FunctionPointerType>
    concept ValidValueOrInstance = ValidValue<Type, FunctionPointerType> || ValidInstance<Type, FunctionPointerType>;
}

#endif // ENGINE_SOURCES_EVENTS_TRAITS_FILE_FUNCTION_POINTER_TRAITS_H
