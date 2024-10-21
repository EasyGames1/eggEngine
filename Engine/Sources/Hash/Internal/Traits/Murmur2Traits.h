#ifndef ENGINE_SOURCES_HASH_TRAITS_FILE_MURMUR2_H
#define ENGINE_SOURCES_HASH_TRAITS_FILE_MURMUR2_H

#include <cstdint>
#include <ranges>

namespace egg::Hash::Internal
{
    template <typename>
    struct BasicMurmur2Traits;


    template <>
    struct BasicMurmur2Traits<std::uint32_t>
    {
        using Type = std::uint32_t;
        static constexpr Type Multiplier { 0x5bd1e995u };
        static constexpr Type BlockSize { sizeof(Type) };
        static constexpr Type ShiftBits { 24u };
        static constexpr Type ByteShift { 3u };
        static constexpr Type FirstFinalShiftBits { 13u };
        static constexpr Type SecondFinalShiftBits { 15u };
    };


    template <>
    struct BasicMurmur2Traits<std::uint64_t>
    {
        using Type = std::uint64_t;
        static constexpr Type Multiplier { (0xc6a4a793ull << 32ull) + static_cast<Type>(BasicMurmur2Traits<std::uint32_t>::Multiplier) };
    };


    template <typename SizeType>
    class Murmur2Traits : public BasicMurmur2Traits<SizeType>
    {
        using BaseType = BasicMurmur2Traits<SizeType>;

    public:
        using Type = SizeType;

        static constexpr Type Multiplier { BaseType::Multiplier };

        [[nodiscard]] static constexpr Type UnalignedLoad(const std::byte* Pointer) noexcept
        {
            Type Result {};
            for (std::size_t i = 0u; i < sizeof(Result); ++i)
            {
                Result |= std::to_integer<Type>(Pointer[i]) << (i << 3u);
            }
            return Result;
        }

        [[nodiscard]] static constexpr Type ShiftMix(const Type Value) noexcept
        {
            return Value ^ Value >> 47u;
        }

        [[nodiscard]] static constexpr Type LoadBytes(const std::span<const std::byte> Bytes)
        {
            Type Result {};
            for (const auto Byte : Bytes | std::views::reverse)
            {
                Result <<= 8u;
                Result += std::to_integer<Type>(Byte);
            }
            return Result;
        }
    };
}

#endif // ENGINE_SOURCES_HASH_TRAITS_FILE_MURMUR2_H
