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
    template <typename SizeType>
    class DataHash
    {
    public:
        explicit constexpr DataHash(const std::span<const std::byte> Data, const SizeType Seed = 0xc70f6907ull)
            : Data { Data }, Seed { Seed }
        {
        }

        [[nodiscard]] constexpr std::uint32_t Murmur() noexcept requires std::is_same_v<SizeType, std::uint32_t>
        {
            constexpr std::uint32_t Multiplier { 0x5bd1e995u };
            constexpr std::uint32_t BlockSize { 4u };

            std::uint32_t Hash { Seed ^ static_cast<std::uint32_t>(Data.size()) };

            const std::byte* Buffer { Data.data() };
            const std::byte* const End { Buffer + Data.size() - BlockSize };

            for (; Buffer <= End; Buffer += BlockSize)
            {
                const std::uint32_t Chunk { UnalignedLoad(Buffer) * Multiplier };
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

        [[nodiscard]] constexpr std::uint64_t Murmur() noexcept requires std::is_same_v<SizeType, std::uint64_t>
        {
            constexpr std::uint64_t Multiplier { 0xc6a4a7935bd1e995ull };
            constexpr std::uint64_t AlignMask { 0x7ull };

            const std::byte* const End { Data.data() + (Data.size() & ~AlignMask) };
            std::uint64_t Hash { Seed ^ Data.size() * Multiplier };

            for (const std::byte* Buffer = Data.data(); Buffer != End; Buffer += 8)
            {
                Hash = (Hash ^ ShiftMix(UnalignedLoad(Buffer) * Multiplier) * Multiplier) * Multiplier;
            }

            if (Data.size() & AlignMask)
            {
                Hash = (Hash ^ LoadBytes({ End, Data.size() & AlignMask })) * Multiplier;
            }

            return ShiftMix(ShiftMix(Hash) * Multiplier);
        }

    private:
        [[nodiscard]] constexpr SizeType UnalignedLoad(const std::byte* Pointer)
        {
            SizeType Result;
            std::memcpy(&Result, Pointer, sizeof(Result));
            return Result;
        }

        [[nodiscard]] static constexpr SizeType LoadBytes(const std::span<const std::byte> Bytes)
        {
            SizeType Result {};

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

        const std::span<const std::byte> Data;
        const SizeType Seed;
    };
}

#endif // ENGINE_SOURCES_HASH_FILE_ALGORITHMS_H
