#ifndef ENGINE_SOURCES_EVENTS_TRAITS_INTERNAL_FILE_FUNCTOR_OVERLOAD_TRAITS_H
#define ENGINE_SOURCES_EVENTS_TRAITS_INTERNAL_FILE_FUNCTOR_OVERLOAD_TRAITS_H

#include <Events/Traits/FunctionPointerTraits.h>
#include <Types/Deduction/Deduction.h>

#include <type_traits>

namespace egg::Events::Internal
{
    template <typename Functor, typename Return, typename... Args> requires std::is_invocable_r_v<Return, Functor, Args...>
    struct FunctorOverloadTraits
    {
        using Type = std::invoke_result_t<Functor, Args...>(Functor::*)(Args...);
    };

    template <typename MemberFunctor, typename Return, ValidInstance<MemberFunctor>, typename... Args> requires
        std::is_member_object_pointer_v<MemberFunctor> &&
        std::is_invocable_r_v<Return, typename Types::MemberObjectPointerTraits<MemberFunctor>::Member, Args...>
    struct MemberFunctorOverloadTraits
    {
        using Type = typename FunctorOverloadTraits<typename Types::MemberObjectPointerTraits<MemberFunctor>::Member, Return, Args...>::Type;
    };
}

#endif // ENGINE_SOURCES_EVENTS_TRAITS_INTERNAL_FILE_FUNCTOR_OVERLOAD_TRAITS_H
