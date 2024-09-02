#ifndef ENGINE_SOURCES_HASH_FILE_MURMUR_H
#define ENGINE_SOURCES_HASH_FILE_MURMUR_H

#include "Hash.h"

#include <cstddef>

namespace egg::Hash
{
    template <typename CharType>
    class BasicHashString final
    {
    public:
        explicit BasicHashString(const std::basic_string_view<CharType> String, const std::size_t Seed = 0xc70f6907ull)
            : String { String }, Seed { Seed }
        {
        }

        template <typename SizeType>
        [[nodiscard]] constexpr SizeType GetMurmur()
        {
            return Murmur<SizeType>(std::as_bytes(std::span { String }), Seed);
        }

    private:
        const std::basic_string_view<CharType> String;
        const std::size_t Seed;
    };

    using HashString = BasicHashString<char>;
    using WideHashString = BasicHashString<wchar_t>;
    using Unicode8HashString = BasicHashString<char8_t>;
    using Unicode16HashString = BasicHashString<char16_t>;
    using Unicode32HashString = BasicHashString<char32_t>;
}

#endif // ENGINE_SOURCES_HASH_FILE_MURMUR_H
