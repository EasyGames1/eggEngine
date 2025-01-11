#ifndef ENGINE_SOURCES_EVENTS_FILE_DELEGATE_H
#define ENGINE_SOURCES_EVENTS_FILE_DELEGATE_H

#include <Config/Config.h>
#include <Events/ConnectionArgument.h>
#include <Events/Traits/FunctionPointerTraits.h>
#include <Events/Traits/FunctorOverloadTraits.h>
#include <Types/Constness.h>


#include <functional>
#include <tuple>
#include <utility>

namespace egg::Events
{
    template <typename>
    class Delegate;

    template <typename ReturnType, typename... Args>
    class Delegate<ReturnType(Args...)>
    {
    public:
        using ResultType = ReturnType;
        using Signature = ResultType(Args...);
        using WrappedSignature = ResultType(const void*, Args...);


        constexpr Delegate() noexcept = default;

        template <auto Candidate>
        constexpr explicit Delegate(ConnectionArgumentType<Candidate>) noexcept
        {
            Connect<Candidate>();
        }

        template <auto Candidate, typename Type> requires ValidValueOrInstance<Type&, decltype(Candidate)>
        constexpr Delegate(ConnectionArgumentType<Candidate>, Type& ValueOrInstance) noexcept
        {
            Connect<Candidate>(ValueOrInstance);
        }

        template <auto Candidate, typename Type> requires ValidValueOrInstance<Type*, decltype(Candidate)>
        constexpr Delegate(ConnectionArgumentType<Candidate>, Type* ValueOrInstance) noexcept
        {
            Connect<Candidate>(ValueOrInstance);
        }

        constexpr explicit Delegate(WrappedSignature* WrappedCallable) noexcept
        {
            Connect(WrappedCallable);
        }

        template <typename Type>
        constexpr Delegate(WrappedSignature* WrappedCallable, Type& Payload) noexcept
        {
            Connect(WrappedCallable, Payload);
        }

        template <auto Candidate>
        constexpr void Connect() noexcept
        {
            Instance = nullptr;
            Callable = GetWrapped<Candidate>();
        }

        template <auto Candidate, typename Type> requires ValidValueOrInstance<Type&, decltype(Candidate)>
        constexpr void Connect(Type& ValueOrInstance) noexcept
        {
            Instance = &ValueOrInstance;
            Callable = GetWrapped<Candidate, Type&>();
        }

        template <auto Candidate, typename Type> requires ValidValueOrInstance<Type*, decltype(Candidate)>
        constexpr void Connect(Type* ValueOrInstance) noexcept
        {
            Instance = ValueOrInstance;
            Callable = GetWrapped<Candidate, Type*>();
        }

        constexpr void Connect(WrappedSignature* WrappedCallable) noexcept
        {
            EGG_ASSERT(WrappedCallable, "Uninitialized function pointer");
            Instance = nullptr;
            Callable = WrappedCallable;
        }

        template <typename Type>
        constexpr void Connect(WrappedSignature* WrappedCallable, Type& Payload) noexcept
        {
            EGG_ASSERT(WrappedCallable, "Uninitialized function pointer");
            Instance = &Payload;
            Callable = WrappedCallable;
        }

        constexpr void Reset() noexcept
        {
            Instance = nullptr;
            Callable = nullptr;
        }

        [[nodiscard]] constexpr WrappedSignature* GetFunction() const noexcept
        {
            return Callable;
        }

        [[nodiscard]] constexpr const void* GetInstance() const noexcept
        {
            return Instance;
        }

        constexpr ResultType operator()(Args... Arguments) const
            noexcept(std::is_nothrow_invocable_r_v<ResultType, WrappedSignature, const void*, Args...>)
        {
            EGG_ASSERT(static_cast<bool>(*this), "Uninitialized delegate");
            return std::invoke_r<ResultType>(Callable, Instance, std::forward<Args>(Arguments)...);
        }

        [[nodiscard]] constexpr explicit operator bool() const noexcept
        {
            return Callable;
        }

        [[nodiscard]] constexpr bool operator==(const Delegate& Other) const noexcept
        {
            return Callable == Other.Callable && Instance == Other.Instance;
        }

        [[nodiscard]] constexpr bool operator!=(const Delegate& Other) const noexcept
        {
            return !(*this == Other);
        }

    private:
        template <auto Candidate, std::size_t InstanceIndex, std::size_t... Indices>
        [[nodiscard]] static constexpr ResultType InvokeMemberFunctor(auto&& Tuple)
        {
            return std::invoke_r<ResultType>(
                std::invoke(
                    Candidate,
                    std::forward<std::tuple_element_t<InstanceIndex, std::tuple<Args...>>>(std::get<InstanceIndex>(Tuple))
                ),
                std::forward<std::tuple_element_t<Indices, std::tuple<Args...>>>(std::get<Indices>(Tuple))...
            );
        }

        template <auto Candidate, std::size_t... Indices>
        [[nodiscard]] static constexpr auto Wrap(std::index_sequence<Indices...>) noexcept
        {
            return [](const void*, Args... Arguments) constexpr -> ResultType
            {
                constexpr std::size_t Offset {
                    !std::is_invocable_r_v<ResultType, decltype(Candidate), std::tuple_element_t<Indices, std::tuple<Args...>>...>
                    * (sizeof...(Args) - sizeof...(Indices))
                };

                [[maybe_unused]] const std::tuple Tuple { std::forward_as_tuple(std::forward<Args>(Arguments)...) };

                if constexpr (ValidMemberFunctorOverload<decltype(Candidate), ResultType, Args...>)
                {
                    return InvokeMemberFunctor<Candidate, Indices + Offset...>(std::move(Tuple));
                }
                else
                {
                    return std::invoke_r<ResultType>(
                        Candidate,
                        std::forward<std::tuple_element_t<Indices + Offset, std::tuple<Args...>>>(std::get<Indices + Offset>(Tuple))...
                    );
                }
            };
        }

        template <auto Candidate, ValidValueOrInstance<decltype(Candidate)> Type, std::size_t... Indices>
        [[nodiscard]] static constexpr auto Wrap(std::index_sequence<Indices...>) noexcept
        {
            return [](const void* Payload, Args... Arguments) constexpr -> ResultType
            {
                constexpr std::size_t Offset {
                    !std::is_invocable_r_v<ResultType, decltype(Candidate), Type, std::tuple_element_t<Indices, std::tuple<Args...>>...>
                    * (sizeof...(Args) - sizeof...(Indices))
                };

                [[maybe_unused]] const std::tuple Tuple { std::forward_as_tuple(std::forward<Args>(Arguments)...) };

                using PayloadConstness = Types::PointeeConstnessAsOther<Type, void>;

                if constexpr (ValidMemberFunctorOverload<decltype(Candidate), ReturnType, Type, Args...>)
                {
                    return std::invoke_r<ResultType>(
                        std::invoke(
                            Candidate,
                            *static_cast<std::conditional_t<std::is_pointer_v<Type>, Type, std::add_pointer_t<Type>>>(
                                const_cast<PayloadConstness>(Payload)
                            )
                        ),
                        std::forward<std::tuple_element_t<Indices + Offset, std::tuple<Args...>>>(std::get<Indices + Offset>(Tuple))...
                    );
                }
                else if constexpr (std::is_pointer_v<Type>)
                {
                    return std::invoke_r<ResultType>(
                        Candidate,
                        static_cast<Type>(const_cast<PayloadConstness>(Payload)),
                        std::forward<std::tuple_element_t<Indices + Offset, std::tuple<Args...>>>(std::get<Indices + Offset>(Tuple))...
                    );
                }
                else
                {
                    return std::invoke_r<ResultType>(
                        Candidate,
                        *static_cast<std::add_pointer_t<Type>>(const_cast<PayloadConstness>(Payload)),
                        std::forward<std::tuple_element_t<Indices + Offset, std::tuple<Args...>>>(std::get<Indices + Offset>(Tuple))...
                    );
                }
            };
        }

        template <auto Candidate>
        [[nodiscard]] static constexpr auto GetWrapped() noexcept
        {
            if constexpr (
                constexpr bool MemberFunctor { ValidMemberFunctorOverload<decltype(Candidate), ResultType, Args...> };
                std::is_invocable_r_v<ResultType, decltype(Candidate), Args...> && !MemberFunctor)
            {
                return [](const void*, Args... Arguments) constexpr -> ResultType
                {
                    return std::invoke_r<ResultType>(Candidate, std::forward<Args>(Arguments)...);
                };
            }
            else if constexpr (std::is_member_pointer_v<decltype(Candidate)>)
            {
                static_assert(sizeof...(Args), "A member pointer provided without an object argument to a delegate that has no arguments");

                using InstanceType = std::tuple_element_t<0u, std::tuple<Args...>>;

                static_assert(
                    ValidInstance<InstanceType, decltype(Candidate)>,
                    "A member pointer provided without an object argument to a delegate whose first argument type differs from the object argument type"
                );

                if constexpr (MemberFunctor)
                {
                    return Wrap<Candidate>(
                        typename FunctionPointerTraits<
                            MemberFunctorOverload<decltype(Candidate), ReturnType, Args...>
                        >::template IndexSequenceFor<InstanceType> {}
                    );
                }
                else
                {
                    return Wrap<Candidate>(
                        typename FunctionPointerTraits<decltype(Candidate)>::template IndexSequenceFor<InstanceType> {}
                    );
                }
            }
            else
            {
                return Wrap<Candidate>(
                    typename FunctionPointerTraits<decltype(Candidate)>::template IndexSequenceFor<> {}
                );
            }
        }

        template <auto Candidate, ValidValueOrInstance<decltype(Candidate)> Type>
        [[nodiscard]] static constexpr auto GetWrapped() noexcept
        {
            if constexpr (ValidMemberFunctorOverload<decltype(Candidate), ResultType, Type, Args...>)
            {
                return Wrap<Candidate, Type>(
                    typename FunctionPointerTraits<
                        MemberFunctorOverload<decltype(Candidate), ResultType, Type, Args...>
                    >::template IndexSequenceFor<> {}
                );
            }
            else if constexpr (std::is_invocable_r_v<ResultType, decltype(Candidate), Type, Args...>)
            {
                return Wrap<Candidate, Type>(std::index_sequence_for<Args...> {});
            }
            else
            {
                return Wrap<Candidate, Type>(
                    typename FunctionPointerTraits<
                        decltype(Candidate),
                        Type
                    >::template IndexSequenceFor<> {}
                );
            }
        }

        const void* Instance {};
        WrappedSignature* Callable {};
    };


    template <auto Candidate>
    Delegate(ConnectionArgumentType<Candidate>) -> Delegate<typename FunctionPointerTraits<decltype(Candidate)>::Type>;

    template <auto Candidate, typename Type> requires ValidValueOrInstance<Type&, decltype(Candidate)>
    Delegate(ConnectionArgumentType<Candidate>, Type&) -> Delegate<typename FunctionPointerTraits<decltype(Candidate), Type&>::Type>;

    template <auto Candidate, typename Type> requires ValidValueOrInstance<Type*, decltype(Candidate)>
    Delegate(ConnectionArgumentType<Candidate>, Type*) -> Delegate<typename FunctionPointerTraits<decltype(Candidate), Type*>::Type>;

    template <typename ReturnType, typename... Args>
    Delegate(ReturnType (*)(const void*, Args...)) -> Delegate<ReturnType(Args...)>;

    template <typename ReturnType, typename... Args, typename Type>
    Delegate(ReturnType (*)(const void*, Args...), Type&) -> Delegate<ReturnType(Args...)>;
}

#endif // ENGINE_SOURCES_EVENTS_FILE_DELEGATE_H
