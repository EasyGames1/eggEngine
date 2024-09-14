#ifndef ENGINE_SOURCES_EVENTS_FILE_DELEGATE_H
#define ENGINE_SOURCES_EVENTS_FILE_DELEGATE_H

#include "Traits/FunctionPointerTraits.h"

#include <functional>
#include <tuple>
#include <utility>

namespace egg::Events
{
    template <auto>
    struct ConnectionArgument
    {
        explicit ConnectionArgument() = default;
    };

    template <typename>
    class Delegate;

    template <typename ReturnType, typename... Args>
    class Delegate<ReturnType(Args...)>
    {
    public:
        using FunctionType = ReturnType(const void*, Args...);
        using Type = ReturnType(Args...);
        using ResultType = ReturnType;

        Delegate() noexcept = default;

        template <auto Candidate>
        explicit Delegate(ConnectionArgument<Candidate>) noexcept
        {
            Connect<Candidate>();
        }

        template <auto Candidate, typename Type>
        Delegate(ConnectionArgument<Candidate>, Type& ValueOrInstance) noexcept
        {
            Connect<Candidate>(ValueOrInstance);
        }

        template <auto Candidate, typename Type>
        Delegate(ConnectionArgument<Candidate>, Type* ValueOrInstance) noexcept
        {
            Connect<Candidate>(ValueOrInstance);
        }

        explicit Delegate(FunctionType* Function, const void* Payload = nullptr) noexcept
        {
            Connect(Function, Payload);
        }

        template <auto Candidate>
        void Connect() noexcept
        {
            Instance = nullptr;
            Function = GetWrapped<Candidate>();
        }

        template <auto Candidate, typename Type>
        void Connect(Type& ValueOrInstance) noexcept
        {
            Instance = &ValueOrInstance;
            Function = GetWrapped<Candidate, Type&>();
        }

        template <auto Candidate, typename Type>
        void Connect(Type* ValueOrInstance) noexcept
        {
            Instance = ValueOrInstance;
            Function = GetWrapped<Candidate, Type*>();
        }

        void Connect(FunctionType* Function, const void* Payload = nullptr) noexcept
        {
            EGG_ASSERT(Function, "Uninitialized function pointer");
            Instance = Payload;
            this->Function = Function;
        }

        void Reset() noexcept
        {
            Instance = nullptr;
            Function = nullptr;
        }

        [[nodiscard]] FunctionType* GetFunction() const noexcept
        {
            return Function;
        }

        [[nodiscard]] const void* GetInstance() const noexcept
        {
            return Instance;
        }

        ReturnType operator()(Args... Arguments) const
        {
            EGG_ASSERT(static_cast<bool>(*this), "Uninitialized delegate");
            return Function(Instance, std::forward<Args>(Arguments)...);
        }

        [[nodiscard]] explicit operator bool() const noexcept
        {
            return Function;
        }

        [[nodiscard]] bool operator==(const Delegate& Other) const noexcept
        {
            return Function == Other.Function && Instance == Other.Instance;
        }

        [[nodiscard]] bool operator!=(const Delegate& Other) const noexcept
        {
            return !(*this == Other);
        }

    private:
        template <auto Candidate, std::size_t... Indices>
        [[nodiscard]] static auto Wrap(std::index_sequence<Indices...>) noexcept
        {
            return [](const void*, Args... Arguments) -> ReturnType
            {
                constexpr std::size_t Offset {
                    !std::is_invocable_r_v<ReturnType, decltype(Candidate), std::tuple_element_t<Indices, std::tuple<Args...>>...>
                    * (sizeof...(Args) - sizeof...(Indices))
                };

                [[maybe_unused]] const std::tuple Tuple { std::forward_as_tuple(std::forward<Args>(Arguments)...) };

                return std::invoke_r<ReturnType>(
                    Candidate,
                    std::forward<std::tuple_element_t<Indices + Offset, std::tuple<Args...>>>(Tuple)...
                );
            };
        }

        template <auto Candidate, typename Type, std::size_t... Indices>
        [[nodiscard]] static auto Wrap(std::index_sequence<Indices...>) noexcept
        {
            return [](const void* Payload, Args... Arguments) -> ReturnType
            {
                constexpr std::size_t Offset {
                    !std::is_invocable_r_v<ReturnType, decltype(Candidate), Type, std::tuple_element_t<Indices, std::tuple<Args...>>...>
                    * (sizeof...(Args) - sizeof...(Indices))
                };

                [[maybe_unused]] const std::tuple Tuple { std::forward_as_tuple(std::forward<Args>(Arguments)...) };

                if constexpr (std::is_pointer_v<Type>)
                {
                    return std::invoke_r<ReturnType>(
                        Candidate,
                        static_cast<Type>(
                            const_cast<std::add_pointer_t<std::conditional_t<
                                    std::is_const_v<std::remove_pointer_t<Type>>,
                                    std::add_const_t<void>,
                                    void>
                            >>(Payload)
                        ),
                        std::forward<std::tuple_element_t<Indices + Offset, std::tuple<Args...>>>(std::get<Indices + Offset>(Tuple))...
                    );
                }
                else
                {
                    return std::invoke_r<ReturnType>(
                        Candidate,
                        *static_cast<std::add_pointer_t<Type>>(
                            const_cast<std::add_pointer_t<std::conditional_t<std::is_const_v<Type>, std::add_const_t<void>, void>>>(Payload)
                        ),
                        std::forward<std::tuple_element_t<Indices + Offset, std::tuple<Args...>>>(std::get<Indices + Offset>(Tuple))...
                    );
                }
            };
        }

        template <auto Candidate>
        [[nodiscard]] static auto GetWrapped() noexcept
        {
            if constexpr (std::is_invocable_r_v<ReturnType, decltype(Candidate), Args...>)
            {
                return [](const void*, Args... Arguments) -> ReturnType
                {
                    return std::invoke_r<ReturnType>(Candidate, std::forward<Args>(Arguments)...);
                };
            }
            else if constexpr (std::is_member_pointer_v<decltype(Candidate)>)
            {
                static_assert(sizeof...(Args), "A member pointer provided without an object argument to a delegate that has no arguments");

                using InstanceType = std::tuple_element_t<0u, std::tuple<Args...>>;
                using TraitsType = FunctionPointerTraits<decltype(Candidate)>;

                static_assert(
                    std::is_same_v<InstanceType, typename TraitsType::InstanceType>,
                    "A member pointer provided without an object argument to a delegate whose first argument type differs from the object argument type"
                );

                return Wrap<Candidate>(
                    typename TraitsType::template IndexSequenceFor<InstanceType> {}
                );
            }
            else
            {
                return Wrap<Candidate>(
                    typename FunctionPointerTraits<decltype(Candidate)>::template IndexSequenceFor<> {}
                );
            }
        }

        template <auto Candidate, typename Type>
        [[nodiscard]] static auto GetWrapped() noexcept
        {
            if constexpr (std::is_invocable_r_v<ReturnType, decltype(Candidate), Type, Args...>)
            {
                return Wrap<Candidate, Type>(std::index_sequence_for<Args...> {});
            }
            else
            {
                return Wrap<Candidate, Type>(
                    typename FunctionPointerTraits<decltype(Candidate)>::template IndexSequenceFor<> {}
                );
            }
        }

        const void* Instance {};
        FunctionType* Function {};
    };

    template <auto Candidate>
    Delegate(ConnectionArgument<Candidate>) -> Delegate<typename FunctionPointerTraits<decltype(Candidate)>::Type>;

    template <auto Candidate, typename Type>
    Delegate(ConnectionArgument<Candidate>, Type&) -> Delegate<typename FunctionPointerTraits<decltype(Candidate)>::Type>;

    template <auto Candidate, typename Type>
    Delegate(ConnectionArgument<Candidate>, Type*) -> Delegate<typename FunctionPointerTraits<decltype(Candidate)>::Type>;

    template <typename ReturnType, typename... Args>
    Delegate(ReturnType (*)(const void*, Args...), const void* = nullptr) -> Delegate<ReturnType(Args...)>;
}

#endif // ENGINE_SOURCES_EVENTS_FILE_DELEGATE_H
