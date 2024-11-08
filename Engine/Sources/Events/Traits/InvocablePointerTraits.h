#ifndef ENGINE_SOURCES_EVENTS_TRAITS_FILE_FUNCTION_POINTER_TRAITS_H
#define ENGINE_SOURCES_EVENTS_TRAITS_FILE_FUNCTION_POINTER_TRAITS_H

#include "Internal/FunctionPointerTraits.h"
#include "Type/Traits/Capabilities.h"

namespace egg::Events
{
    template <typename InvocablePointer, typename... Type>
    using InvocablePointerTraits = Internal::FunctionPointerTraits<std::remove_cv_t<InvocablePointer>, Type...>;

    template <typename Type>
    concept FreeFunctionPointer = !requires { typename InvocablePointerTraits<Type>::InstanceType; };

    template <typename Type, typename InvocablePointerType>
    concept ValidValue =
        FreeFunctionPointer<InvocablePointerType> &&
        std::convertible_to<Type, typename InvocablePointerTraits<InvocablePointerType, Type>::InstanceType>;

    template <typename Type, typename InvocablePointerType>
    concept ValidInstance =
        std::convertible_to<std::remove_pointer_t<Type>, typename InvocablePointerTraits<InvocablePointerType>::InstanceType>;

    template <typename Type, typename InvocablePointerType>
    concept ValidValueOrInstance = ValidValue<Type, InvocablePointerType> || ValidInstance<Type, InvocablePointerType>;
}

#endif // ENGINE_SOURCES_EVENTS_TRAITS_FILE_FUNCTION_POINTER_TRAITS_H
