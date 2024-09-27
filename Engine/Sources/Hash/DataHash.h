#ifndef ENGINE_SOURCES_HASH_FILE_ALGORITHMS_H
#define ENGINE_SOURCES_HASH_FILE_ALGORITHMS_H

#include "./Internal/Traits/FNV1aTraits.h"
#include "./Internal/Traits/Murmur2Traits.h"

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <ranges>

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

        [[nodiscard]] constexpr std::uint32_t Murmur2() const noexcept requires std::is_same_v<SizeType, std::uint32_t>
        {
            using TraitsType = Internal::Murmur2Traits<SizeType>;

            constexpr SizeType BlockSize { TraitsType::BlockSize };

            SizeType Hash { Seed ^ static_cast<SizeType>(Data.size()) };

            const std::byte* Buffer { Data.data() };
            const std::byte* const End { Buffer + Data.size() - BlockSize };

            for (; Buffer <= End; Buffer += BlockSize)
            {
                const SizeType Chunk { TraitsType::UnalignedLoad(Buffer) * TraitsType::Multiplier };
                Hash = Hash * TraitsType::Multiplier ^ (Chunk ^ (Chunk >> 24u) * TraitsType::Multiplier);
            }

            const SizeType Remainder { static_cast<SizeType>(std::distance(Buffer, End)) };
            for (SizeType i = Remainder; i--;)
            {
                Hash ^= std::to_integer<SizeType>(Buffer[i]) << (i << 3u);
            }

            if (Remainder) Hash *= TraitsType::Multiplier;

            Hash = (Hash ^ Hash >> 13u) * TraitsType::Multiplier;

            return Hash ^ Hash >> 15u;
        }

        [[nodiscard]] constexpr SizeType Murmur2() const noexcept requires std::is_same_v<SizeType, std::uint64_t>
        {
            using TraitsType = Internal::Murmur2Traits<SizeType>;

            constexpr SizeType AlignMask { 0x7ull };

            const std::byte* const End { Data.data() + (Data.size() & ~AlignMask) };
            SizeType Hash { Seed ^ Data.size() * TraitsType::Multiplier };

            for (const std::byte* Buffer = Data.data(); Buffer != End; Buffer += 8)
            {
                Hash ^= TraitsType::ShiftMix(TraitsType::UnalignedLoad(Buffer) * TraitsType::Multiplier) * TraitsType::Multiplier;
                Hash *= TraitsType::Multiplier;
            }

            if (Data.size() & AlignMask)
            {
                Hash = (Hash ^ TraitsType::LoadBytes({ End, Data.size() & AlignMask })) * TraitsType::Multiplier;
            }

            return TraitsType::ShiftMix(TraitsType::ShiftMix(Hash) * TraitsType::Multiplier);
        }

        [[nodiscard]] constexpr SizeType FNV1a() const noexcept
        {
            using TraitsType = Internal::FNV1aTraits<SizeType>;

            SizeType Hash { TraitsType::Offset };

            for (auto Current : Data)
            {
                Hash = (Hash ^ std::to_integer<SizeType>(Current)) * TraitsType::Prime;
            }

            return Hash;
        }

    private:
        const std::span<const std::byte> Data;
        const SizeType Seed;
    };
}

#endif // ENGINE_SOURCES_HASH_FILE_ALGORITHMS_H
