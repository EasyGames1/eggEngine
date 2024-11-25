#ifndef ENGINE_SOURCES_MEMORY_FILE_UTILS_H
#define ENGINE_SOURCES_MEMORY_FILE_UTILS_H

#include <Config/Config.h>
#include <Memory/Constants.h>
#include <Memory/Deleter.h>

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

    template <std::unsigned_integral Type, Types::Byte ByteType>
    [[nodiscard]] constexpr Type UnalignedLoad(const ByteType* Bytes) noexcept
    {
        Type Result {};
        for (Type i = 0u; i < sizeof(Result); ++i)
        {
            Result |= static_cast<Type>(Bytes[i]) << ToBits(i);
        }
        return Result;
    }

    template <std::unsigned_integral Type, Types::Byte ByteType>
    [[nodiscard]] constexpr Type LoadBytes(const std::span<const ByteType> Bytes) noexcept
    {
        EGG_ASSERT(Bytes.size() <= sizeof(Type), "The Type cannot accommodate so many Bytes");
        Type Result {};
        for (const auto Byte : Bytes | std::views::reverse)
        {
            Result <<= Memory::BitsIn<static_cast<Type>(1u)>;
            Result |= static_cast<Type>(Byte);
        }
        return Result;
    }

    template <std::unsigned_integral Type>
    [[nodiscard]] constexpr Type ShiftMix(const Type Value, const Type ShiftBits) noexcept
    {
        return Value ^ Value >> ShiftBits;
    }

    template <typename Type, typename AllocatorParameter, typename... Args> requires (!std::is_array_v<Type>)
    [[nodiscard]] constexpr std::unique_ptr<
        Type,
        AllocationDeleter<typename std::allocator_traits<AllocatorParameter>::template rebind_alloc<Type>>
    > AllocateUnique(const AllocatorParameter& Allocator, Args&&... Arguments)
    {
        using UniqueAllocatorTraits = typename std::allocator_traits<AllocatorParameter>::template rebind_traits<Type>;
        using AllocatorType = typename UniqueAllocatorTraits::allocator_type;
        using PointerType = typename UniqueAllocatorTraits::pointer;

        AllocatorType AllocatorCopy { Allocator };
        PointerType Pointer { UniqueAllocatorTraits::allocate(AllocatorCopy, 1u) };

        try
        {
            UniqueAllocatorTraits::construct(AllocatorCopy, std::to_address(Pointer), std::forward<Args>(Arguments)...);
        }
        catch (...)
        {
            UniqueAllocatorTraits::deallocate(AllocatorCopy, Pointer, 1u);
            throw;
        }

        return std::unique_ptr<Type, AllocationDeleter<AllocatorType>> {
            Pointer,
            AllocationDeleter<AllocatorType> { AllocatorCopy }
        };
    }
}

#endif // ENGINE_SOURCES_MEMORY_FILE_UTILS_H
