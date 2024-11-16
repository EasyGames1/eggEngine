#ifndef ENGINE_SOURCES_HASH_TRAITS_FILE_FNV1A_TRAITS_H
#define ENGINE_SOURCES_HASH_TRAITS_FILE_FNV1A_TRAITS_H

#include <cstdint>

namespace egg::Hash::Internal
{
    template <std::unsigned_integral>
    struct FNV1aTraits;


    template <>
    struct FNV1aTraits<std::uint32_t>
    {
        using SizeType = std::uint32_t;
        static constexpr SizeType Seed { 0x811c9dc5u };
        static constexpr SizeType Prime { 0x01000193u };
    };


    template <>
    struct FNV1aTraits<std::uint64_t>
    {
        using SizeType = std::uint64_t;
        static constexpr SizeType Seed { 0xcbf29ce484222325ull };
        static constexpr SizeType Prime { 0x100000001b3ull };
    };
}

#endif // ENGINE_SOURCES_HASH_TRAITS_FILE_FNV1A_TRAITS_H
