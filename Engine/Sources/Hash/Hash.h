#ifndef ENGINE_SOURCES_HASH_FILE_ALGORITHMS_H
#define ENGINE_SOURCES_HASH_FILE_ALGORITHMS_H

#include "./Traits/FNV1aTraits.h"
#include "./Traits/Murmur2Traits.h"
#include "Memory/Utils.h"

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <ranges>

namespace egg::Hash
{
    template <std::same_as<std::uint32_t> SizeType>
    [[nodiscard]] constexpr SizeType Murmur2(const std::span<const std::byte> Data,
                                             const SizeType Seed = Internal::Murmur2Traits<SizeType>::Seed) noexcept
    {
        using TraitsType = Internal::Murmur2Traits<SizeType>;

        SizeType Hash = Seed ^ static_cast<SizeType>(Data.size());

        const std::byte* First { Data.data() };
        const std::byte* const End { Data.data() + Data.size() };

        for (const std::byte* Last = End - TraitsType::BlockSize; First < Last; First += TraitsType::BlockSize)
        {
            const SizeType Chunk { Memory::UnalignedLoad<SizeType>(First) * TraitsType::Multiplier };
            Hash *= TraitsType::Multiplier;
            Hash ^= Memory::ShiftMix(Chunk, TraitsType::ShiftBits) * TraitsType::Multiplier;
        }

        if (auto Remainder = static_cast<SizeType>(std::distance(First, End)))
        {
            while (Remainder--)
            {
                Hash ^= std::to_integer<SizeType>(First[Remainder]) << Memory::ToBits(Remainder);
            }
            Hash *= TraitsType::Multiplier;
        }

        Hash = Memory::ShiftMix(Hash, TraitsType::FirstFinalShiftBits);
        Hash *= TraitsType::Multiplier;
        Hash = Memory::ShiftMix(Hash, TraitsType::SecondFinalShiftBits);

        return Hash;
    }

    template <std::same_as<std::uint64_t> SizeType>
    [[nodiscard]] constexpr SizeType Murmur2(const std::span<const std::byte> Data,
                                             const SizeType Seed = Internal::Murmur2Traits<SizeType>::Seed) noexcept
    {
        using TraitsType = Internal::Murmur2Traits<SizeType>;

        constexpr SizeType AlignMask { 0x7ull };

        SizeType Hash { Seed ^ Data.size() * TraitsType::Multiplier };

        const std::byte* const End { Data.data() + (Data.size() & ~AlignMask) };

        for (const std::byte* First = Data.data(); First != End; First += 8)
        {
            const SizeType Chunk { Memory::UnalignedLoad<SizeType>(First) * TraitsType::Multiplier };
            Hash ^= Memory::ShiftMix(Chunk, TraitsType::ShiftBits) * TraitsType::Multiplier;
            Hash *= TraitsType::Multiplier;
        }

        if (Data.size() & AlignMask)
        {
            Hash ^= Memory::LoadBytes<SizeType>({ End, Data.size() & AlignMask });
            Hash *= TraitsType::Multiplier;
        }

        Hash = Memory::ShiftMix(Hash, TraitsType::ShiftBits);
        Hash *= TraitsType::Multiplier;
        Hash = Memory::ShiftMix(Hash, TraitsType::ShiftBits);

        return Hash;
    }

    template <std::unsigned_integral SizeType>
    [[nodiscard]] constexpr SizeType FNV1a(const std::span<const std::byte> Data,
                                           SizeType Hash = Internal::FNV1aTraits<SizeType>::Seed) noexcept
    {
        using TraitsType = Internal::FNV1aTraits<SizeType>;

        for (const auto Byte : Data)
        {
            Hash ^= std::to_integer<SizeType>(Byte);
            Hash *= TraitsType::Prime;
        }

        return Hash;
    }

    template <std::unsigned_integral SizeType>
    [[nodiscard]] constexpr SizeType Hash(const std::span<const std::byte> Data) noexcept
    {
        return Murmur2<SizeType>(Data);
    }
}

#endif // ENGINE_SOURCES_HASH_FILE_ALGORITHMS_H
