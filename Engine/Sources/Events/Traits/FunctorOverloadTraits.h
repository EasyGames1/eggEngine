#ifndef ENGINE_SOURCES_EVENTS_TRAITS_FILE_FUNCTOR_OVERLOAD_TRAITS_H
#define ENGINE_SOURCES_EVENTS_TRAITS_FILE_FUNCTOR_OVERLOAD_TRAITS_H

#include "./Internal/FunctorOverloadTraits.h"

#include <Config/Config.h>

namespace egg::Events
{
    template <typename Functor, typename Return, typename... Args>
    concept ValidFunctorOverload = requires { typename Internal::FunctorOverloadTraits<Functor, Return, Args...>; };

    template <typename Functor, typename Return, typename... Args> requires ValidFunctorOverload<Functor, Return, Args...>
    using FunctorOverload = typename Internal::FunctorOverloadTraits<Functor, Return, Args...>::Type;

    template <typename MemberFunctor, typename Return, typename... Args>
    concept ValidMemberFunctorOverload =
        EGG_EVENTS_RESOLVE_MEMBER_FUNCTORS && requires { typename Internal::MemberFunctorOverloadTraits<MemberFunctor, Return, Args...>; };

    template <typename MemberFunctor, typename Return, typename... Args> requires ValidMemberFunctorOverload<MemberFunctor, Return, Args...>
    using MemberFunctorOverload = typename Internal::MemberFunctorOverloadTraits<MemberFunctor, Return, Args...>::Type;
}

#endif // ENGINE_SOURCES_EVENTS_TRAITS_FILE_FUNCTOR_OVERLOAD_TRAITS_H
