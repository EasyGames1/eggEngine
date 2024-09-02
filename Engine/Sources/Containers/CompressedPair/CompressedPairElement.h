#ifndef ENGINE_SOURCES_CONTAINERS_COMPRESSED_PAIR_FILE_COMPRESSED_PAIR_ELEMENT_H
#define ENGINE_SOURCES_CONTAINERS_COMPRESSED_PAIR_FILE_COMPRESSED_PAIR_ELEMENT_H

#include "../Container.h"

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

namespace egg::Containers
{
    template <typename Type, std::size_t>
    class CompressedPairElement
    {
    public:
        using Reference = Type&;
        using ConstReference = const Type&;

        constexpr CompressedPairElement()
            noexcept (std::is_nothrow_default_constructible_v<Type>)
            requires (std::is_default_constructible_v<Type>)
            : Value {}
        {
        }

        template <typename Arg> requires (!std::is_same_v<std::remove_cv_t<std::remove_reference_t<Arg>>, CompressedPairElement>)
        explicit constexpr CompressedPairElement(Arg&& Argument) noexcept (std::is_nothrow_constructible_v<Type, Arg>)
            : Value { std::forward<Arg>(Argument) }
        {
        }

        template <typename... Args, std::size_t ... Indices>
        constexpr CompressedPairElement(std::tuple<Args...> Arguments,
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

    template <EBCOEligible Type, std::size_t Position>
    class CompressedPairElement<Type, Position> : public Type
    {
        using BaseType = Type;

    public:
        using Reference = Type&;
        using ConstReference = const Type&;

        constexpr CompressedPairElement()
            noexcept (std::is_nothrow_default_constructible_v<BaseType>)
            requires (std::is_default_constructible_v<BaseType>)
            : BaseType {}
        {
        }

        template <typename Arg> requires (!std::is_same_v<std::remove_cv_t<std::remove_reference_t<Arg>>, CompressedPairElement>)
        explicit constexpr CompressedPairElement(Arg&& Argument) noexcept (std::is_nothrow_constructible_v<BaseType, Arg>)
            : BaseType { std::forward<Arg>(Argument) }
        {
        }

        template <typename... Args, std::size_t... Indices>
        constexpr CompressedPairElement(std::tuple<Args...> Arguments,
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
