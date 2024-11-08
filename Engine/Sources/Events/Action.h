#ifndef ENGINE_SOURCES_EVENTS_FILE_CONNECTION_H
#define ENGINE_SOURCES_EVENTS_FILE_CONNECTION_H

#include "Delegate.h"
#include "Config/Config.h"
#include "Containers/CompressedPair/CompressedPair.h"

#include <tuple>

namespace egg::Events
{
    template <typename>
    class Action;

    template <typename ReturnType, typename... Args>
    class Action<ReturnType(Args...)>
    {
        using ArgumentsTuple = std::tuple<Args...>;

    public:
        using DelegateType = Delegate<ReturnType(Args...)>;
        using ResultType = typename DelegateType::ResultType;
        using Signature = typename DelegateType::Signature;


        template <std::convertible_to<Args>...Other>
        constexpr explicit Action(Other&&... Arguments) noexcept (std::is_nothrow_constructible_v<ArgumentsTuple, Other&&...>)
            : Callable {
                std::piecewise_construct,
                std::forward_as_tuple(),
                std::forward_as_tuple(std::forward<Other>(Arguments)...)
            }
        {
        }

        template <std::convertible_to<Args>...Other>
        constexpr explicit Action(DelegateType Invocable, Other&&... Arguments)
            noexcept (std::is_nothrow_constructible_v<ArgumentsTuple, Other&&...>)
            : Callable {
                std::piecewise_construct,
                std::forward_as_tuple(Invocable),
                std::forward_as_tuple(std::forward<Other>(Arguments)...)
            }
        {
        }

        constexpr ResultType operator()() const noexcept(std::is_nothrow_invocable_r_v<ResultType, DelegateType, Args...>)
        {
            EGG_ASSERT(static_cast<bool>(Callable.GetFirst()), "Uninitialized Invocable");
            return std::apply(Callable.GetFirst(), Callable.GetSecond());
        }

        [[nodiscard]] constexpr DelegateType& GetInvocable() noexcept
        {
            return Callable.GetFirst();
        }

        [[nodiscard]] constexpr const DelegateType& GetInvocable() const noexcept
        {
            return Callable.GetFirst();
        }

    private:
        Containers::CompressedPair<DelegateType, ArgumentsTuple> Callable;
    };

    template <typename ReturnType, typename... Args, std::convertible_to<Args>...Other>
    Action(Delegate<ReturnType(Args...)>, Other&&...) -> Action<ReturnType(Args...)>;
}

#endif // ENGINE_SOURCES_EVENTS_FILE_CONNECTION_H
