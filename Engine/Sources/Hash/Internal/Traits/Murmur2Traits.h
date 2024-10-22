#ifndef ENGINE_SOURCES_HASH_TRAITS_FILE_MURMUR2_H
#define ENGINE_SOURCES_HASH_TRAITS_FILE_MURMUR2_H

#include "Constants/Memory.h"
#include "Utils/Memory.h"

#include <cstdint>

namespace egg::Hash::Internal
{
    template <std::unsigned_integral>
    struct Murmur2Traits;


    template <>
    struct Murmur2Traits<std::uint32_t>
    {
        using Type = std::uint32_t;

        static constexpr Type Multiplier { 0x5bd1e995u };
        static constexpr Type BlockSize { sizeof(Type) };
        static constexpr Type ShiftBits { Constants::Memory::BitsIn<3u> };
        static constexpr Type FirstFinalShiftBits { 13u };
        static constexpr Type SecondFinalShiftBits { 15u };
    };


    template <>
    struct Murmur2Traits<std::uint64_t>
    {
        using Type = std::uint64_t;

        static constexpr Type Multiplier {
            (0xc6a4a793ull << Constants::Memory::BitsIn<4ull>) + static_cast<Type>(Murmur2Traits<std::uint32_t>::Multiplier)
        };
        static constexpr Type ShiftBits { 47u };
    };
}

#endif // ENGINE_SOURCES_HASH_TRAITS_FILE_MURMUR2_H
