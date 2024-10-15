#ifndef ENGINE_SOURCES_EVENTS_FILE_SIGNAL_H
#define ENGINE_SOURCES_EVENTS_FILE_SIGNAL_H

#include "Delegate.h"
#include "Type/Traits/Capabilities.h"

#include <memory>
#include <vector>

namespace egg::Events
{
    template <typename Type, typename AllocatorParameter = std::allocator<Delegate<Type>>>
    class Signal;

    template <typename ReturnType, typename... Args, Types::ValidAllocator<Delegate<ReturnType(Args...)>> AllocatorParameter>
    class Signal<ReturnType(Args...), AllocatorParameter>
    {
        using DelegateType = Delegate<ReturnType(Args...)>;

        using ContainerAllocatorTraits = std::allocator_traits<AllocatorParameter>;
        using ContainerType = std::vector<DelegateType, AllocatorParameter>;

    public:
        using AllocatorType = AllocatorParameter;


        constexpr Signal()
            noexcept(std::is_nothrow_constructible_v<AllocatorType> && std::is_nothrow_constructible_v<ContainerType, const AllocatorType&>)
            : Signal { AllocatorType {} }
        {
        }

        constexpr explicit Signal(
            const AllocatorType& Allocator) noexcept(std::is_nothrow_constructible_v<ContainerType, const AllocatorType&>)
            : Slots { Allocator }
        {
        }

        constexpr Signal(const Signal&) noexcept(std::is_nothrow_copy_constructible_v<ContainerType>) = default;

        constexpr Signal(const Signal& Other, const AllocatorType& Allocator)
            noexcept(std::is_nothrow_constructible_v<ContainerType, const ContainerType&, const AllocatorType&>)
            : Slots { Other.Slots, Allocator }
        {
        }

        constexpr Signal(Signal&& Other) noexcept(std::is_nothrow_move_constructible_v<ContainerType>) = default;

        constexpr Signal(Signal&& Other, const AllocatorType& Allocator)
            noexcept(std::is_nothrow_constructible_v<ContainerType, ContainerType&&, const AllocatorType&>)
            : Slots { std::move(Other.Slots), Allocator }
        {
        }

        constexpr Signal& operator=(const Signal&) noexcept(std::is_nothrow_copy_assignable_v<ContainerType>) = default;

        Signal& operator=(Signal&&) noexcept(std::is_nothrow_move_assignable_v<ContainerType>) = default;

        friend void swap(Signal& Left, Signal& Right) noexcept(std::is_nothrow_swappable_v<ContainerType>)
        {
            using std::swap;
            swap(Left.Slots, Right.Slots);
        }

    private:
        ContainerType Slots;
    };
}

#endif // ENGINE_SOURCES_EVENTS_FILE_SIGNAL_H
