#ifndef ENGINE_SOURCES_EVENTS_FILE_SIGNAL_H
#define ENGINE_SOURCES_EVENTS_FILE_SIGNAL_H

#include "Action.h"
#include "Delegate.h"
#include "Type/Traits/Capabilities.h"

#include <memory>
#include <vector>

namespace egg::Events
{
    template <typename Type, typename = std::allocator<Delegate<Type>>>
    class Signal;


    template <typename ReturnType, typename... Args, typename AllocatorParameter>
    class Signal<ReturnType(Args...), AllocatorParameter>
    {
        using SlotType = Delegate<ReturnType(Args...)>;

        using ContainerAllocatorTraits = std::allocator_traits<AllocatorParameter>;
        using ContainerType = std::vector<SlotType, typename ContainerAllocatorTraits::template rebind_alloc<SlotType>>;

    public:
        using AllocatorType = AllocatorParameter;

        using ResultType = typename SlotType::ResultType;
        using Signature = typename SlotType::Signature;
        using WrappedSignature = typename SlotType::WrappedSignature;


        constexpr Signal()
            noexcept(
                std::is_nothrow_default_constructible_v<AllocatorType> &&
                std::is_nothrow_constructible_v<ContainerType, const AllocatorType&>)
            : Signal { AllocatorType {} }
        {
        }

        constexpr explicit Signal(const AllocatorType& Allocator)
            noexcept(std::is_nothrow_constructible_v<ContainerType, const AllocatorType&>)
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

        constexpr Signal& operator=(Signal&&) noexcept(std::is_nothrow_move_assignable_v<ContainerType>) = default;

        constexpr friend void swap(Signal& Left, Signal& Right) noexcept(std::is_nothrow_swappable_v<ContainerType>)
        {
            using std::swap;
            swap(Left.Slots, Right.Slots);
        }

        template <auto Candidate>
        constexpr Action<void()> Connect()
        {
            return ConnectDelegate<Candidate>();
        }

        template <auto Candidate, typename Type> requires ValidValueOrInstance<Type&, decltype(Candidate)>
        constexpr Action<void(Type&)> Connect(Type& ValueOrInstance)
        {
            return ConnectDelegate<Candidate>(ValueOrInstance);
        }

        template <auto Candidate, typename Type> requires ValidValueOrInstance<Type*, decltype(Candidate)>
        constexpr Action<void(Type*)> Connect(Type* ValueOrInstance)
        {
            return ConnectDelegate<Candidate>(std::move(ValueOrInstance));
        }

        constexpr Action<void(WrappedSignature*)> Connect(WrappedSignature* WrappedFunction)
        {
            return ConnectDelegate(std::move(WrappedFunction));
        }

        template <typename Type>
        constexpr Action<void(WrappedSignature*, Type&)> Connect(WrappedSignature* WrappedFunction, Type& Payload)
        {
            return ConnectDelegate(std::move(WrappedFunction), Payload);
        }

        constexpr void Publish(Args... Arguments) const
        {
            for (std::size_t Position = GetSize(); Position--;)
            {
                Slots[Position](Arguments...);
            }
        }

        template <typename FunctorType> requires
            (!std::is_void_v<ReturnType> && std::invocable<FunctorType, ReturnType>) || std::is_invocable_v<FunctorType>
        constexpr void Collect(FunctorType Collector, Args... Arguments) const
        {
            for (std::size_t Position = GetSize(); Position--;)
            {
                if constexpr (!std::is_void_v<ReturnType> && std::invocable<FunctorType, ReturnType>)
                {
                    if constexpr (std::is_invocable_r_v<bool, FunctorType, ReturnType>)
                    {
                        if (Collector(Slots[Position](Arguments...))) break;
                    }
                    else
                    {
                        Collector(Slots[Position](Arguments...));
                    }
                }
                else
                {
                    Slots[Position](Arguments...);

                    if constexpr (std::is_invocable_r_v<bool, FunctorType>)
                    {
                        if (Collector()) break;
                    }
                    else
                    {
                        Collector();
                    }
                }
            }
        }

        template <auto Candidate>
        constexpr void Disconnect()
        {
            DisconnectDelegate(ConnectionArgument<Candidate>);
        }

        template <auto Candidate, typename Type> requires ValidValueOrInstance<Type&, decltype(Candidate)>
        constexpr void Disconnect(Type& ValueOrInstance)
        {
            DisconnectDelegate(ConnectionArgument<Candidate>, ValueOrInstance);
        }

        template <auto Candidate, typename Type> requires ValidValueOrInstance<Type*, decltype(Candidate)>
        constexpr void Disconnect(Type* ValueOrInstance)
        {
            DisconnectDelegate(ConnectionArgument<Candidate>, ValueOrInstance);
        }

        constexpr void Disconnect(WrappedSignature* WrappedFunction)
        {
            DisconnectDelegate(WrappedFunction);
        }

        template <typename Type>
        constexpr void Disconnect(WrappedSignature* WrappedFunction, Type& Payload)
        {
            DisconnectDelegate(WrappedFunction, Payload);
        }

        template <typename Type>
        constexpr void Disconnect(Type& ValueOrInstance)
        {
            const Type* const Pointer { &ValueOrInstance };
            DisconnectIf([Pointer](const SlotType& Element) -> bool { return Element.GetInstance() == Pointer; });
        }

        constexpr void Disconnect() noexcept
        {
            Slots.clear();
        }

        [[nodiscard]] constexpr std::size_t GetSize() const noexcept
        {
            return Slots.size();
        }

        [[nodiscard]] constexpr bool Empty() const noexcept
        {
            return Slots.empty();
        }

        [[nodiscard]] constexpr AllocatorType GetAllocator() const noexcept
        {
            return Slots.get_allocator();
        }

    private:
        template <typename FunctorType> requires std::is_invocable_r_v<bool, FunctorType, typename ContainerType::const_reference>
        constexpr void DisconnectIf(FunctorType Callback)
        {
            for (std::size_t Position = GetSize(); Position--;)
            {
                if (auto& Element { Slots[Position] }; Callback(std::as_const(Element)))
                {
                    Element = std::move(Slots.back());
                    Slots.pop_back();
                }
            }
        }

        template <auto Candidate, typename... DelegateArgs>
        constexpr Action<void(DelegateArgs...)> ConnectDelegate(DelegateArgs&&... Arguments)
        {
            Disconnect<Candidate>(std::forward<DelegateArgs>(Arguments)...);
            Slots.emplace_back(ConnectionArgument<Candidate>, std::forward<DelegateArgs>(Arguments)...);
            return Action {
                Delegate {
                    ConnectionArgument<static_cast<void(Signal::*)(DelegateArgs...)>(&Signal::Disconnect<Candidate>)>,
                    *this
                },
                std::forward<DelegateArgs>(Arguments)...
            };
        }

        template <typename... DelegateArgs>
        constexpr Action<void(DelegateArgs...)> ConnectDelegate(DelegateArgs&&... Arguments)
        {
            Disconnect(std::forward<DelegateArgs>(Arguments)...);
            Slots.emplace_back(std::forward<DelegateArgs>(Arguments)...);
            return Action {
                Delegate {
                    ConnectionArgument<static_cast<void(Signal::*)(DelegateArgs...)>(&Signal::Disconnect)>,
                    *this
                },
                std::forward<DelegateArgs>(Arguments)...
            };
        }

        template <typename... DelegateArgs>
        constexpr void DisconnectDelegate(DelegateArgs&&... Arguments)
        {
            SlotType Call { std::forward<DelegateArgs>(Arguments)... };
            DisconnectIf([Call](const auto& Element) -> bool { return Element == Call; });
        }

        ContainerType Slots;
    };
}

#endif // ENGINE_SOURCES_EVENTS_FILE_SIGNAL_H
