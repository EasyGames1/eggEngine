#ifndef ENGINE_SOURCES_HASH_FILE_ALGORITHMS_H
#define ENGINE_SOURCES_HASH_FILE_ALGORITHMS_H

#include "./Internal/Traits/FNV1aTraits.h"
#include "./Internal/Traits/Murmur2Traits.h"

#include <cstddef>
#include <cstdint>
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

        [[nodiscard]] constexpr SizeType Murmur2() const noexcept requires std::is_same_v<SizeType, std::uint32_t>
        {
            using TraitsType = Internal::Murmur2Traits<SizeType>;

            SizeType Hash = Seed ^ static_cast<SizeType>(Data.size());

            const std::byte* First { Data.data() };
            const std::byte* End { Data.data() + Data.size() };

            for (const std::byte* Last = End - TraitsType::BlockSize; First < Last; First += TraitsType::BlockSize)
            {
                const SizeType Chunk { TraitsType::UnalignedLoad(First) * TraitsType::Multiplier };
                Hash = Hash * TraitsType::Multiplier ^ (Chunk ^ Chunk >> TraitsType::ShiftBits) * TraitsType::Multiplier;
            }

            if (auto Remainder { static_cast<SizeType>(std::distance(First, End)) })
            {
                while (Remainder--)
                {
                    Hash ^= std::to_integer<SizeType>(First[Remainder]) << (Remainder << TraitsType::ByteShift);
                }
                Hash *= TraitsType::Multiplier;
            }

            Hash = (Hash ^ Hash >> TraitsType::FirstFinalShiftBits) * TraitsType::Multiplier;

            return Hash ^ Hash >> TraitsType::SecondFinalShiftBits;
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

            for (const auto Byte : Data)
            {
                Hash = (Hash ^ std::to_integer<SizeType>(Byte)) * TraitsType::Prime;
            }

            return Hash;
        }

    private:
        const std::span<const std::byte> Data;
        const SizeType Seed;
    };
}

#endif // ENGINE_SOURCES_HASH_FILE_ALGORITHMS_H
