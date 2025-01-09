#ifndef ENGINE_SOURCES_HASH_FILE_ALGORITHMS_H
#define ENGINE_SOURCES_HASH_FILE_ALGORITHMS_H

#include <Hash/Traits/FNV1aTraits.h>
#include <Hash/Traits/Murmur2Traits.h>
#include <Memory/Constants.h>
#include <Memory/Utils.h>

#include <concepts>
#include <cstdint>

namespace egg::Hash
{
    template <std::same_as<std::uint32_t> SizeType, Types::Byte ByteType>
    [[nodiscard]] constexpr SizeType Murmur2(const std::span<const ByteType> Data,
                                             const SizeType Seed = Internal::Murmur2Traits<SizeType>::Seed) noexcept
    {
        using TraitsType = Internal::Murmur2Traits<SizeType>;

        SizeType Hash = Seed ^ static_cast<SizeType>(Data.size());

        const ByteType* First { Data.data() };
        const ByteType* const End { Data.data() + Data.size() };

        for (const ByteType* Last = End - TraitsType::BlockSize; First < Last; First += TraitsType::BlockSize)
        {
            const SizeType Chunk { Memory::UnalignedLoad<SizeType>(First) * TraitsType::Multiplier };
            Hash *= TraitsType::Multiplier;
            Hash ^= Memory::ShiftMix(Chunk, TraitsType::ShiftBits) * TraitsType::Multiplier;
        }

        if (auto Remainder { static_cast<SizeType>(std::distance(First, End)) })
        {
            while (Remainder--)
            {
                Hash ^= static_cast<SizeType>(First[Remainder]) << Memory::ToBits(Remainder);
            }
            Hash *= TraitsType::Multiplier;
        }

        Hash = Memory::ShiftMix(Hash, TraitsType::FirstFinalShiftBits);
        Hash *= TraitsType::Multiplier;
        Hash = Memory::ShiftMix(Hash, TraitsType::SecondFinalShiftBits);

        return Hash;
    }

    template <std::same_as<std::uint64_t> SizeType, Types::Byte ByteType>
    [[nodiscard]] constexpr SizeType Murmur2(const std::span<const ByteType> Data,
                                             const SizeType Seed = Internal::Murmur2Traits<SizeType>::Seed) noexcept
    {
        using TraitsType = Internal::Murmur2Traits<SizeType>;

        SizeType Hash { Seed ^ Data.size() * TraitsType::Multiplier };

        const ByteType* const End { Data.data() + (Data.size() & ~TraitsType::AlignMask) };

        for (const ByteType* First = Data.data(); First != End; First += Memory::BitsIn<1u>)
        {
            const SizeType Chunk { Memory::UnalignedLoad<SizeType>(First) * TraitsType::Multiplier };
            Hash ^= Memory::ShiftMix(Chunk, TraitsType::ShiftBits) * TraitsType::Multiplier;
            Hash *= TraitsType::Multiplier;
        }

        if (Data.size() & TraitsType::AlignMask)
        {
            Hash ^= Memory::LoadBytes<SizeType>({ End, Data.size() & TraitsType::AlignMask });
            Hash *= TraitsType::Multiplier;
        }

        Hash = Memory::ShiftMix(Hash, TraitsType::ShiftBits);
        Hash *= TraitsType::Multiplier;
        Hash = Memory::ShiftMix(Hash, TraitsType::ShiftBits);

        return Hash;
    }

    template <std::unsigned_integral SizeType, Types::Byte ByteType>
    [[nodiscard]] constexpr SizeType FNV1a(const std::span<const ByteType> Data,
                                           SizeType Seed = Internal::FNV1aTraits<SizeType>::Seed) noexcept
    {
        using TraitsType = Internal::FNV1aTraits<SizeType>;

        for (const auto Byte : Data)
        {
            Seed ^= static_cast<SizeType>(Byte);
            Seed *= TraitsType::Prime;
        }

        return Seed;
    }

    template <std::unsigned_integral SizeType, Types::Byte ByteType>
    [[nodiscard]] constexpr SizeType Hash(const std::span<const ByteType> Data) noexcept
    {
        return Murmur2<SizeType>(Data);
    }
}

#endif // ENGINE_SOURCES_HASH_FILE_ALGORITHMS_H
