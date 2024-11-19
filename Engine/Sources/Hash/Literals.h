#ifndef ENGINE_SOURCES_HASH_FILE_LITERALS_H
#define ENGINE_SOURCES_HASH_FILE_LITERALS_H

#include "Hash.h"
#include "Types/Types.h"
#include "Types/Traits/Capabilities.h"

#include <span>

namespace egg::Hash::Literals
{
    namespace Internal
    {
        template <Types::Character Char>
        [[nodiscard]] constexpr Types::HashType HashString(const Char* String, const std::size_t Size) noexcept
        {
            return Hash::Hash<Types::HashType>(std::as_bytes(std::span { String, Size }));
        }
    }

    [[nodiscard]] constexpr Types::HashType operator""_hs(const char* String, const std::size_t Size) noexcept
    {
        return Internal::HashString(String, Size);
    }

    [[nodiscard]] constexpr Types::HashType operator""_hs(const wchar_t* String, const std::size_t Size) noexcept
    {
        return Internal::HashString(String, Size);
    }

    [[nodiscard]] constexpr Types::HashType operator""_hs(const char8_t* String, const std::size_t Size) noexcept
    {
        return Internal::HashString(String, Size);
    }

    [[nodiscard]] constexpr Types::HashType operator""_hs(const char16_t* String, const std::size_t Size) noexcept
    {
        return Internal::HashString(String, Size);
    }

    [[nodiscard]] constexpr Types::HashType operator""_hs(const char32_t* String, const std::size_t Size) noexcept
    {
        return Internal::HashString(String, Size);
    }
}

#endif // ENGINE_SOURCES_HASH_FILE_LITERALS_H
