#ifndef ENGINE_SOURCES_CONTAINERS_COMPRESSED_PAIR_FILE_COMPRESSED_PAIR_ELEMENT_H
#define ENGINE_SOURCES_CONTAINERS_COMPRESSED_PAIR_FILE_COMPRESSED_PAIR_ELEMENT_H

#include "Types/Traits/Capabilities.h"

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

namespace egg::Containers::Internal
{
    template <typename Type, std::size_t>
    class CompressedPairElement
    {
    public:
        using Reference = Type&;
        using ConstReference = const Type&;

        constexpr CompressedPairElement()
            noexcept (std::is_nothrow_default_constructible_v<Type>)
            requires (std::default_initializable<Type>)
            : Value {}
        {
        }

        template <typename Arg> requires (!std::same_as<std::remove_cvref_t<Arg>, CompressedPairElement>)
        constexpr explicit CompressedPairElement(Arg&& Argument) noexcept (std::is_nothrow_constructible_v<Type, Arg&&>)
            : Value { std::forward<Arg>(Argument) }
        {
        }

        template <typename... Args, std::size_t ... Indices>
        constexpr CompressedPairElement([[maybe_unused]] std::tuple<Args...> Arguments,
                                        std::index_sequence<Indices...>) noexcept (std::is_nothrow_constructible_v<Type, Args...>)
            : Value { std::forward<Args>(std::get<Indices>(Arguments))... }
        {
        }

        [[nodiscard]] constexpr Reference Get() noexcept
        {
            return Value;
        }

        [[nodiscard]] constexpr ConstReference Get() const noexcept
        {
            return Value;
        }

    private:
        Type Value;
    };


    template <Types::EBCOEligible Type, std::size_t Position>
    class CompressedPairElement<Type, Position> : public Type
    {
        using BaseType = Type;

    public:
        using Reference = Type&;
        using ConstReference = const Type&;

        constexpr CompressedPairElement()
            noexcept (std::is_nothrow_default_constructible_v<BaseType>)
            requires (std::default_initializable<BaseType>)
            : BaseType {}
        {
        }

        template <typename Arg> requires (!std::same_as<std::remove_cvref_t<Arg>, CompressedPairElement>)
        constexpr explicit CompressedPairElement(Arg&& Argument) noexcept (std::is_nothrow_constructible_v<BaseType, Arg&&>)
            : BaseType { std::forward<Arg>(Argument) }
        {
        }

        template <typename... Args, std::size_t... Indices>
        constexpr CompressedPairElement([[maybe_unused]] std::tuple<Args...> Arguments,
                                        std::index_sequence<Indices...>) noexcept (std::is_nothrow_constructible_v<BaseType, Args...>)
            : BaseType { std::forward<Args>(std::get<Indices>(Arguments))... }
        {
        }

        [[nodiscard]] constexpr Reference Get() noexcept
        {
            return *this;
        }

        [[nodiscard]] constexpr ConstReference Get() const noexcept
        {
            return *this;
        }
    };
}


#endif // ENGINE_SOURCES_CONTAINERS_COMPRESSED_PAIR_FILE_COMPRESSED_PAIR_ELEMENT_H
