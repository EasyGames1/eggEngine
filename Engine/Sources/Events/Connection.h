#ifndef ENGINE_SOURCES_EVENTS_FILE_CONNECTION_H
#define ENGINE_SOURCES_EVENTS_FILE_CONNECTION_H

#include "Signal.h"

namespace egg::Events
{
    template <auto, typename, typename...>
    class Connection;


    template <auto Candidate, typename ReturnType, typename... Args, typename AllocatorParameter>
    class Connection<Candidate, Signal<ReturnType(Args...), AllocatorParameter>>
    {
    public:
        using SignalType = Signal<ReturnType(Args...), AllocatorParameter>;


        Connection(ConnectionArgumentType<Candidate>, SignalType& Signal) : Signal { &Signal }
        {
        }

        Connection(ConnectionArgumentType<Candidate>, SignalType* Signal) : Signal { Signal }
        {
        }

        [[nodiscard]] explicit operator bool() const noexcept
        {
            return Signal;
        }

        void Release()
        {
            if (Signal)
            {
                Signal->template Disconnect<Candidate>();
                Signal = nullptr;
            }
        }

    private:
        SignalType* Signal;
    };


    template <auto Candidate, typename ReturnType, typename... Args, typename AllocatorParameter, typename Type>
        requires std::is_lvalue_reference_v<Type> || std::is_pointer_v<Type>
    class Connection<Candidate, Signal<ReturnType(Args...), AllocatorParameter>, Type>
    {
    public:
        using SignalType = Signal<ReturnType(Args...), AllocatorParameter>;


        Connection(ConnectionArgumentType<Candidate>, Type ValueOrInstance, SignalType& Signal)
            : Instance { ValueOrInstance }, Signal { &Signal }
        {
        }

        Connection(ConnectionArgumentType<Candidate>, Type ValueOrInstance, SignalType* Signal)
            : Instance { ValueOrInstance }, Signal { Signal }
        {
        }

        [[nodiscard]] explicit operator bool() const noexcept
        {
            return Signal;
        }

        void Release()
        {
            if (Signal)
            {
                Signal->template Disconnect<Candidate>(Instance);
                Signal = nullptr;
            }
        }

    private:
        Type Instance;
        SignalType* Signal;
    };


    template <auto Candidate, typename ReturnType, typename... Args, typename AllocatorParameter>
    Connection(ConnectionArgumentType<Candidate>, Signal<ReturnType(Args...), AllocatorParameter>&)
        -> Connection<Candidate, Signal<ReturnType(Args...), AllocatorParameter>>;

    template <auto Candidate, typename ReturnType, typename... Args, typename AllocatorParameter>
    Connection(ConnectionArgumentType<Candidate>, Signal<ReturnType(Args...), AllocatorParameter>*)
        -> Connection<Candidate, Signal<ReturnType(Args...), AllocatorParameter>>;


    template <auto Candidate, typename ReturnType, typename... Args, typename AllocatorParameter, typename Type>
        requires std::is_lvalue_reference_v<Type> || std::is_pointer_v<Type>
    Connection(ConnectionArgumentType<Candidate>, Type, Signal<ReturnType(Args...), AllocatorParameter>&)
        -> Connection<Candidate, Signal<ReturnType(Args...), AllocatorParameter>, Type>;

    template <auto Candidate, typename ReturnType, typename... Args, typename AllocatorParameter, typename Type>
        requires std::is_lvalue_reference_v<Type> || std::is_pointer_v<Type>
    Connection(ConnectionArgumentType<Candidate>, Type, Signal<ReturnType(Args...), AllocatorParameter>*)
        -> Connection<Candidate, Signal<ReturnType(Args...), AllocatorParameter>, Type>;


    template <auto Candidate, typename ReturnType, typename... Args, typename AllocatorParameter, typename Type>
        requires (!std::is_lvalue_reference_v<Type> && !std::is_pointer_v<Type>)
    Connection(ConnectionArgumentType<Candidate>, Type, Signal<ReturnType(Args...), AllocatorParameter>&)
        -> Connection<Candidate, Signal<ReturnType(Args...), AllocatorParameter>, std::add_lvalue_reference_t<Type>>;

    template <auto Candidate, typename ReturnType, typename... Args, typename AllocatorParameter, typename Type>
        requires (!std::is_lvalue_reference_v<Type> && !std::is_pointer_v<Type>)
    Connection(ConnectionArgumentType<Candidate>, Type, Signal<ReturnType(Args...), AllocatorParameter>*)
        -> Connection<Candidate, Signal<ReturnType(Args...), AllocatorParameter>, std::add_lvalue_reference_t<Type>>;
}

#endif // ENGINE_SOURCES_EVENTS_FILE_CONNECTION_H
