#ifndef ENGINE_SOURCES_HASH_FILE_ALGORITHMS_H
#define ENGINE_SOURCES_HASH_FILE_ALGORITHMS_H

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <ranges>
#include <utility>

namespace egg::Hash
{
    namespace Internal
    {
        template <typename Type>
        [[nodiscard]] constexpr Type UnalignedLoad(const std::byte* Pointer)
        {
            Type Result;
            std::memcpy(&Result, Pointer, sizeof(Result));
            return Result;
        }

        template <typename Type>
        [[nodiscard]] static constexpr Type LoadBytes(const std::span<const std::byte> Bytes)
        {
            std::uint64_t Result {};

            for (const auto Byte : std::ranges::reverse_view { Bytes })
            {
                Result = (Result << 8) + static_cast<unsigned char>(Byte);
            }

            return Result;
        }

        template <typename Type>
        [[nodiscard]] static constexpr Type ShiftMix(const Type Value)
        {
            return Value ^ Value >> 47;
        }
    }

    template <typename Type>
    [[nodiscard]] constexpr Type Murmur(std::span<const std::byte> Data, Type Seed) noexcept = delete;

    template <>
    [[nodiscard]] constexpr std::uint32_t Murmur<std::uint32_t>(
        const std::span<const std::byte> Data,
        const std::uint32_t Seed) noexcept
    {
        constexpr std::uint32_t Multiplier { 0x5bd1e995u };
        constexpr std::uint32_t BlockSize { 4u };

        std::uint32_t Hash { Seed ^ static_cast<std::uint32_t>(Data.size()) };

        const std::byte* Buffer { Data.data() };
        const std::byte* const End { Buffer + Data.size() - BlockSize };

        for (; Buffer <= End; Buffer += BlockSize)
        {
            const std::uint32_t Chunk { Internal::UnalignedLoad<std::uint32_t>(Buffer) * Multiplier };
            Hash = Hash * Multiplier ^ (Chunk ^ (Chunk >> 24u) * Multiplier);
        }

        const std::uint32_t Remainder { static_cast<std::uint32_t>(std::distance(Buffer, End)) };
        for (std::uint32_t i = Remainder; i--;)
        {
            Hash ^= std::to_integer<std::uint32_t>(Buffer[i]) << (i << 3u);
        }

        if (Remainder) Hash *= Multiplier;

        Hash = (Hash ^ Hash >> 13u) * Multiplier;

        return Hash ^ Hash >> 15u;
    }

    template <>
    [[nodiscard]] constexpr std::uint64_t Murmur<std::uint64_t>(
        const std::span<const std::byte> Data,
        const std::uint64_t Seed) noexcept
    {
        constexpr std::uint64_t Multiplier { 0xc6a4a7935bd1e995ull };
        constexpr std::uint64_t AlignMask { 0x7ull };

        const std::byte* const End { Data.data() + (Data.size() & ~AlignMask) };
        std::uint64_t Hash { Seed ^ Data.size() * Multiplier };

        for (const std::byte* Buffer = Data.data(); Buffer != End; Buffer += 8)
        {
            Hash = (Hash ^ Internal::ShiftMix(Internal::UnalignedLoad<std::uint64_t>(Buffer) * Multiplier) * Multiplier) * Multiplier;
        }

        if (Data.size() & AlignMask)
        {
            Hash = (Hash ^ Internal::LoadBytes<std::uint64_t>({ End, Data.size() & AlignMask })) * Multiplier;
        }

        return Internal::ShiftMix(Internal::ShiftMix(Hash) * Multiplier);
    }
}

#endif // ENGINE_SOURCES_HASH_FILE_ALGORITHMS_H
