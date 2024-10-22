#ifndef ENGINE_SOURCES_UTILS_FILE_MEMORY_H
#define ENGINE_SOURCES_UTILS_FILE_MEMORY_H

#include "Constants/Memory.h"

#include <concepts>
#include <ranges>

namespace egg::Utils::Memory
{
    template <std::unsigned_integral Type>
    [[nodiscard]] constexpr Type ToBits(Type Bytes) noexcept
    {
        return Bytes << Constants::Memory::ByteShift;
    }

    template <std::unsigned_integral Type>
    [[nodiscard]] constexpr Type ToBytes(Type Bits) noexcept
    {
        return Bits >> Constants::Memory::ByteShift;
    }

    template <std::unsigned_integral Type>
    [[nodiscard]] constexpr Type UnalignedLoad(const std::byte* Pointer) noexcept
    {
        Type Result {};
        for (Type i = 0u; i < sizeof(Result); ++i)
        {
            Result |= std::to_integer<Type>(Pointer[i]) << ToBits(i);
        }
        return Result;
    }

    template <std::unsigned_integral Type>
    [[nodiscard]] constexpr Type LoadBytes(const std::span<const std::byte> Bytes) noexcept
    {
        Type Result {};
        for (const auto Byte : Bytes | std::views::reverse)
        {
            Result <<= Constants::Memory::BitsIn<static_cast<Type>(1u)>;
            Result += std::to_integer<Type>(Byte);
        }
        return Result;
    }

    template <std::unsigned_integral Type>
    [[nodiscard]] constexpr Type ShiftMix(const Type Value, const Type ShiftBits) noexcept
    {
        return Value ^ Value >> ShiftBits;
    }
}

#endif // ENGINE_SOURCES_UTILS_FILE_MEMORY_H
