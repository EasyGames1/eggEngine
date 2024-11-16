#ifndef ENGINE_SOURCES_MEMORY_FILE_UTILS_H
#define ENGINE_SOURCES_MEMORY_FILE_UTILS_H

#include "./Constants.h"
#include "./Deleter.h"
#include "Config/Config.h"
#include "Type/Traits/Capabilities.h"

#include <concepts>
#include <ranges>

namespace egg::Memory
{
    template <std::unsigned_integral Type>
    [[nodiscard]] constexpr Type ToBits(Type Bytes) noexcept
    {
        return Bytes << ByteShift;
    }

    template <std::unsigned_integral Type>
    [[nodiscard]] constexpr Type ToBytes(Type Bits) noexcept
    {
        return Bits >> ByteShift;
    }

    template <std::unsigned_integral Type>
    [[nodiscard]] constexpr Type UnalignedLoad(const std::byte* Bytes) noexcept
    {
        Type Result {};
        for (Type i = 0u; i < sizeof(Result); ++i)
        {
            Result |= std::to_integer<Type>(Bytes[i]) << ToBits(i);
        }
        return Result;
    }

    template <std::unsigned_integral Type>
    [[nodiscard]] constexpr Type LoadBytes(const std::span<const std::byte> Bytes) noexcept
    {
        EGG_ASSERT(Bytes.size() <= sizeof(Type), "The Type cannot accommodate so many Bytes");
        Type Result {};
        for (const auto Byte : Bytes | std::views::reverse)
        {
            Result <<= Memory::BitsIn<static_cast<Type>(1u)>;
            Result |= std::to_integer<Type>(Byte);
        }
        return Result;
    }

    template <std::unsigned_integral Type>
    [[nodiscard]] constexpr Type ShiftMix(const Type Value, const Type ShiftBits) noexcept
    {
        return Value ^ Value >> ShiftBits;
    }

    template <typename Type, Types::ValidAllocator<Type> AllocatorType, typename... Args>
    [[nodiscard]] constexpr std::unique_ptr<Type, Deleter<AllocatorType>>
    AllocateUnique(const AllocatorType& Allocator, Args&&... Arguments)
    {
        using Traits = std::allocator_traits<AllocatorType>;
        using Pointer = typename Traits::pointer;

        AllocatorType AllocatorCopy { Allocator };
        Pointer Allocated { Traits::allocate(AllocatorCopy, 1u) };
        Traits::construct(AllocatorCopy, Allocated, std::forward<Args>(Arguments)...);

        return std::unique_ptr<Type, Deleter<AllocatorType>> {
            Allocated,
            Deleter<AllocatorType> { Allocator }
        };
    }
}

#endif // ENGINE_SOURCES_MEMORY_FILE_UTILS_H
