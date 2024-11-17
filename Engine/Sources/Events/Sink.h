#ifndef ENGINE_SOURCES_EVENTS_FILE_SINK_H
#define ENGINE_SOURCES_EVENTS_FILE_SINK_H

#include "Signal.h"

namespace egg::Events
{
    template <typename>
    class Sink;

    template <typename ReturnType, typename... Args, typename Allocator>
    class Sink<Signal<ReturnType(Args...), Allocator>>
    {
    public:
        using SignalType = Signal<ReturnType(Args...), Allocator>;

        using ResultType = typename SignalType::ResultType;
        using Signature = typename SignalType::Signature;
        using WrappedSignature = typename SignalType::WrappedSignature;


        constexpr explicit Sink(SignalType& Connections) noexcept : Connections { Connections }
        {
        }

        template <auto Candidate>
        constexpr Action<void()> Connect()
        {
            return Connections.template Connect<Candidate>();
        }

        template <auto Candidate, typename Type> requires ValidValueOrInstance<Type&, decltype(Candidate)>
        constexpr Action<void(Type&)> Connect(Type& ValueOrInstance)
        {
            return Connections.template Connect<Candidate>(ValueOrInstance);
        }

        template <auto Candidate, typename Type> requires ValidValueOrInstance<Type*, decltype(Candidate)>
        constexpr Action<void(Type*)> Connect(Type* ValueOrInstance)
        {
            return Connections.template Connect<Candidate>(ValueOrInstance);
        }

        constexpr Action<void(WrappedSignature*)> Connect(WrappedSignature* WrappedFunction)
        {
            return Connections.Connect(WrappedFunction);
        }

        template <typename Type>
        constexpr Action<void(WrappedSignature*, Type&)> Connect(WrappedSignature* WrappedFunction, Type& Payload)
        {
            return Connections.Connect(WrappedFunction, Payload);
        }

        template <auto Candidate>
        constexpr void Disconnect()
        {
            Connections.template Disconnect<Candidate>();
        }

        template <auto Candidate, typename Type> requires ValidValueOrInstance<Type&, decltype(Candidate)>
        constexpr void Disconnect(Type& ValueOrInstance)
        {
            Connections.template Disconnect<Candidate>(ValueOrInstance);
        }

        template <auto Candidate, typename Type> requires ValidValueOrInstance<Type*, decltype(Candidate)>
        constexpr void Disconnect(Type* ValueOrInstance)
        {
            Connections.template Disconnect<Candidate>(ValueOrInstance);
        }

        constexpr void Disconnect(WrappedSignature* WrappedFunction)
        {
            Connections.Disconnect(WrappedFunction);
        }

        template <typename Type>
        constexpr void Disconnect(WrappedSignature* WrappedFunction, Type& Payload)
        {
            Connections.Disconnect(WrappedFunction, Payload);
        }

        template <typename Type>
        constexpr void Disconnect(Type& ValueOrInstance)
        {
            Connections.Disconnect(ValueOrInstance);
        }

        constexpr void Disconnect() noexcept
        {
            Connections.Disconnect();
        }

        [[nodiscard]] constexpr bool Empty() const noexcept
        {
            return Connections.Empty();
        }

    private:
        SignalType& Connections;
    };
}

#endif // ENGINE_SOURCES_EVENTS_FILE_SINK_H
