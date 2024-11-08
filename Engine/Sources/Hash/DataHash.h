#ifndef ENGINE_SOURCES_HASH_FILE_ALGORITHMS_H
#define ENGINE_SOURCES_HASH_FILE_ALGORITHMS_H

#include "./Internal/Traits/FNV1aTraits.h"
#include "./Internal/Traits/Murmur2Traits.h"
#include "Utils/Memory.h"

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <ranges>

namespace egg::Hash
{
    template <std::unsigned_integral SizeType>
    class DataHash
    {
    public:
        constexpr explicit DataHash(const std::span<const std::byte> Data, const SizeType Seed = 0xc70f6907ull)
            : Data { Data }, Seed { Seed }
        {
        }

        [[nodiscard]] constexpr SizeType Murmur2() const noexcept requires std::same_as<SizeType, std::uint32_t>
        {
            using TraitsType = Internal::Murmur2Traits<SizeType>;

            SizeType Hash = Seed ^ static_cast<SizeType>(Data.size());

            const std::byte* First { Data.data() };
            const std::byte* const End { Data.data() + Data.size() };

            for (const std::byte* Last = End - TraitsType::BlockSize; First < Last; First += TraitsType::BlockSize)
            {
                const SizeType Chunk { Utils::Memory::UnalignedLoad<SizeType>(First) * TraitsType::Multiplier };
                Hash *= TraitsType::Multiplier;
                Hash ^= Utils::Memory::ShiftMix(Chunk, TraitsType::ShiftBits) * TraitsType::Multiplier;
            }

            if (auto Remainder = static_cast<SizeType>(std::distance(First, End)))
            {
                while (Remainder--)
                {
                    Hash ^= std::to_integer<SizeType>(First[Remainder]) << Utils::Memory::ToBits(Remainder);
                }
                Hash *= TraitsType::Multiplier;
            }

            Hash = Utils::Memory::ShiftMix(Hash, TraitsType::FirstFinalShiftBits);
            Hash *= TraitsType::Multiplier;
            Hash = Utils::Memory::ShiftMix(Hash, TraitsType::SecondFinalShiftBits);

            return Hash;
        }

        [[nodiscard]] constexpr SizeType Murmur2() const noexcept requires std::same_as<SizeType, std::uint64_t>
        {
            using TraitsType = Internal::Murmur2Traits<SizeType>;

            constexpr SizeType AlignMask { 0x7ull };

            SizeType Hash { Seed ^ Data.size() * TraitsType::Multiplier };

            const std::byte* const End { Data.data() + (Data.size() & ~AlignMask) };

            for (const std::byte* First = Data.data(); First != End; First += 8)
            {
                const SizeType Chunk { Utils::Memory::UnalignedLoad<SizeType>(First) * TraitsType::Multiplier };
                Hash ^= Utils::Memory::ShiftMix(Chunk, TraitsType::ShiftBits) * TraitsType::Multiplier;
                Hash *= TraitsType::Multiplier;
            }

            if (Data.size() & AlignMask)
            {
                Hash ^= Utils::Memory::LoadBytes<SizeType>({ End, Data.size() & AlignMask });
                Hash *= TraitsType::Multiplier;
            }

            Hash = Utils::Memory::ShiftMix(Hash, TraitsType::ShiftBits);
            Hash *= TraitsType::Multiplier;
            Hash = Utils::Memory::ShiftMix(Hash, TraitsType::ShiftBits);

            return Hash;
        }

        [[nodiscard]] constexpr SizeType FNV1a() const noexcept
        {
            using TraitsType = Internal::FNV1aTraits<SizeType>;

            SizeType Hash { TraitsType::Offset };

            for (const auto Byte : Data)
            {
                Hash ^= std::to_integer<SizeType>(Byte);
                Hash *= TraitsType::Prime;
            }

            return Hash;
        }

    private:
        const std::span<const std::byte> Data;
        const SizeType Seed;
    };
}

#endif // ENGINE_SOURCES_HASH_FILE_ALGORITHMS_H
