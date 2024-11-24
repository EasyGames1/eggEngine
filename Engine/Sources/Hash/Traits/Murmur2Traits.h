#ifndef ENGINE_SOURCES_HASH_TRAITS_FILE_MURMUR2_TRAITS_H
#define ENGINE_SOURCES_HASH_TRAITS_FILE_MURMUR2_TRAITS_H

#include <Memory/Constants.h>

#include <concepts>
#include <cstdint>

namespace egg::Hash::Internal
{
    template <std::unsigned_integral>
    struct BasicMurmur2Traits;


    template <>
    struct BasicMurmur2Traits<std::uint32_t>
    {
        using SizeType = std::uint32_t;

        static constexpr SizeType Multiplier { 0x5bd1e995u };
        static constexpr SizeType BlockSize { sizeof(SizeType) };
        static constexpr SizeType ShiftBits { Memory::BitsIn<3u> };
        static constexpr SizeType FirstFinalShiftBits { 13u };
        static constexpr SizeType SecondFinalShiftBits { 15u };
    };


    template <>
    struct BasicMurmur2Traits<std::uint64_t>
    {
        using SizeType = std::uint64_t;

        static constexpr SizeType Multiplier {
            (0xc6a4a793ull << Memory::BitsIn<4ull>) + static_cast<SizeType>(BasicMurmur2Traits<std::uint32_t>::Multiplier)
        };
        static constexpr SizeType ShiftBits { 47u };
    };


    template <std::unsigned_integral SizeParameter>
    class Murmur2Traits : public BasicMurmur2Traits<SizeParameter>
    {
        using BaseType = BasicMurmur2Traits<SizeParameter>;

    public:
        using SizeType = SizeParameter;

        static constexpr SizeType Seed { 0xc70f6907ull };
    };
}

#endif // ENGINE_SOURCES_HASH_TRAITS_FILE_MURMUR2_TRAITS_H
