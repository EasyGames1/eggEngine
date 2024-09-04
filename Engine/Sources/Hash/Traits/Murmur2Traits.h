#ifndef ENGINE_SOURCES_HASH_TRAITS_FILE_MURMUR2_H
#define ENGINE_SOURCES_HASH_TRAITS_FILE_MURMUR2_H

#include <cstdint>
#include <cstring>
#include <ranges>

namespace egg::Hash::Traits
{
    template <typename>
    struct BasicMurmur2Traits;

    template <>
    struct BasicMurmur2Traits<std::uint32_t>
    {
        using Type = std::uint32_t;
        static constexpr Type Multiplier { 0x5bd1e995u };
        static constexpr Type BlockSize { sizeof(Type) };
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

        [[nodiscard]] static constexpr Type UnalignedLoad(const std::byte* Pointer)
        {
            Type Result;
            std::memcpy(&Result, Pointer, sizeof(Result));
            return Result;
        }

        [[nodiscard]] static constexpr Type ShiftMix(const Type Value)
        {
            return Value ^ Value >> 47;
        }

        [[nodiscard]] static constexpr Type LoadBytes(const std::span<const std::byte> Bytes)
        {
            Type Result {};

            for (const auto Byte : std::ranges::reverse_view { Bytes })
            {
                Result = (Result << 8) + static_cast<unsigned char>(Byte);
            }

            return Result;
        }
    };
}

#endif // ENGINE_SOURCES_HASH_TRAITS_FILE_MURMUR2_H
