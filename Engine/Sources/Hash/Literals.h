#ifndef ENGINE_SOURCES_HASH_FILE_LITERALS_H
#define ENGINE_SOURCES_HASH_FILE_LITERALS_H

#include <Hash/Hash.h>
#include <Types/Types.h>

#include <span>

namespace egg::Hash::Literals
{
    [[nodiscard]] constexpr Types::IDType operator""_hs(const char* String, const std::size_t Size) noexcept
    {
        return Hash::Hash<Types::IDType>(std::span { String, Size });
    }
}

#endif // ENGINE_SOURCES_HASH_FILE_LITERALS_H
