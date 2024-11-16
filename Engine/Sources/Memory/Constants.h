#ifndef ENGINE_SOURCES_MEMORY_FILE_CONSTANTS_H
#define ENGINE_SOURCES_MEMORY_FILE_CONSTANTS_H

#include "Math/Traits/Capabilities.h"

#include <bit>
#include <climits>
#include <concepts>
#include <cstddef>

namespace egg::Memory
{
    inline constexpr std::size_t PageSizeInBytes { 16384u };
    static_assert(std::has_single_bit(PageSizeInBytes), "Page size in bytes must be a power of two");

    template <typename Type>
    inline constexpr std::size_t PageSize { PageSizeInBytes / std::bit_ceil(sizeof(Type)) };

    inline constexpr std::size_t ByteMultiplier { CHAR_BIT };
    static_assert(std::has_single_bit(ByteMultiplier), "The number of bits in a byte must be a power of two");

    inline constexpr std::size_t ByteShift { std::countr_zero(ByteMultiplier) };

    template <auto Bytes> requires std::unsigned_integral<decltype(Bytes)>
    inline constexpr decltype(Bytes) BitsIn { Bytes << ByteShift };

    template <auto Bits> requires std::unsigned_integral<decltype(Bits)> && Math::MultipleOf<Bits, ByteMultiplier>
    inline constexpr decltype(Bits) BytesIn { Bits >> ByteShift };
}

#endif // ENGINE_SOURCES_MEMORY_FILE_CONSTANTS_H
