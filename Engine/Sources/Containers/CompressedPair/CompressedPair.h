#ifndef ENGINE_SOURCES_CONTAINERS_COMPRESSED_PAIR_FILE_COMPRESSED_PAIR_H
#define ENGINE_SOURCES_CONTAINERS_COMPRESSED_PAIR_FILE_COMPRESSED_PAIR_H

#include "./Internal/CompressedPairElement.h"

namespace egg::Containers
{
    template <typename First, typename Second>
    class CompressedPair final
        : Internal::CompressedPairElement<First, 0u>,
          Internal::CompressedPairElement<Second, 1u>
    {
        using FirstBase = Internal::CompressedPairElement<First, 0u>;
        using SecondBase = Internal::CompressedPairElement<Second, 1u>;

    public:
        using FirstType = First;
        using SecondType = Second;

        constexpr CompressedPair()
            noexcept (std::is_nothrow_default_constructible_v<FirstBase> && std::is_nothrow_constructible_v<SecondBase>)
            requires (std::is_default_constructible_v<FirstType> && std::is_default_constructible_v<SecondType>)
            : FirstBase {}, SecondBase {}
        {
        }

        constexpr CompressedPair(const CompressedPair& Other)
            noexcept (std::is_nothrow_copy_constructible_v<FirstBase> && std::is_nothrow_copy_constructible_v<SecondBase>) = default;

        constexpr CompressedPair(CompressedPair&& Other)
            noexcept (std::is_nothrow_move_constructible_v<FirstBase> && std::is_nothrow_move_constructible_v<SecondBase>) = default;

        template <typename Arg, typename OtherType>
        constexpr CompressedPair(Arg&& Argument, OtherType&& Other)
            noexcept (std::is_nothrow_constructible_v<FirstBase, Arg> && std::is_nothrow_constructible_v<SecondBase, OtherType>)
            : FirstBase { std::forward<Arg>(Argument) },
              SecondBase { std::forward<OtherType>(Other) }
        {
        }

        template <typename... Args, typename... OtherArgs>
        constexpr CompressedPair(std::piecewise_construct_t,
                                 std::tuple<Args...> Arguments,
                                 std::tuple<OtherArgs...> OtherArguments)
            noexcept (std::is_nothrow_constructible_v<FirstBase, Args...> && std::is_nothrow_constructible_v<SecondBase, OtherArgs...>)
            : FirstBase { std::move(Arguments), std::index_sequence_for<Args...> {} },
              SecondBase { std::move(OtherArguments), std::index_sequence_for<OtherArgs...> {} }
        {
        }

        constexpr CompressedPair& operator=(const CompressedPair& Other)
            noexcept (std::is_nothrow_copy_assignable_v<FirstBase> && std::is_nothrow_copy_assignable_v<SecondBase>) = default;

        constexpr CompressedPair& operator=(CompressedPair&& Other)
            noexcept (std::is_nothrow_move_assignable_v<FirstBase> && std::is_nothrow_move_assignable_v<SecondBase>) = default;

        constexpr friend void swap(CompressedPair& Left, CompressedPair& Right)
            noexcept(std::is_nothrow_swappable_v<FirstType> && std::is_nothrow_swappable_v<SecondType>)
        {
            using std::swap;
            swap(Left.GetFirst(), Right.GetFirst());
            swap(Left.GetSecond(), Right.GetSecond());
        }

        [[nodiscard]] constexpr FirstType& GetFirst() noexcept
        {
            return static_cast<FirstBase&>(*this).Get();
        }

        [[nodiscard]] constexpr const FirstType& GetFirst() const noexcept
        {
            return static_cast<const FirstBase&>(*this).Get();
        }

        [[nodiscard]] constexpr SecondType& GetSecond() noexcept
        {
            return static_cast<SecondBase&>(*this).Get();
        }

        [[nodiscard]] constexpr const SecondType& GetSecond() const noexcept
        {
            return static_cast<const SecondBase&>(*this).Get();
        }

        template <std::size_t Index>
        constexpr decltype(auto) Get() noexcept
        {
            if constexpr (!Index)
            {
                return GetFirst();
            }
            else
            {
                static_assert(Index == 1u, "Index out of bounds");
                return GetSecond();
            }
        }

        template <std::size_t Index>
        constexpr decltype(auto) Get() const noexcept
        {
            if constexpr (!Index)
            {
                return GetFirst();
            }
            else
            {
                static_assert(Index == 1u, "Index out of bounds");
                return GetSecond();
            }
        }
    };

    template <typename Type, typename Other>
    CompressedPair(Type&&, Other&&) -> CompressedPair<std::decay_t<Type>, std::decay_t<Other>>;
}


#endif // ENGINE_SOURCES_CONTAINERS_COMPRESSED_PAIR_FILE_COMPRESSED_PAIR_H
