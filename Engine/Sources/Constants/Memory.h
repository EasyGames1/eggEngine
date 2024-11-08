#ifndef ENGINE_SOURCES_CONSTANTS_FILE_MEMORY_H
#define ENGINE_SOURCES_CONSTANTS_FILE_MEMORY_H

#include "Math/Traits/Capabilities.h"

#include <bit>
#include <climits>
#include <concepts>
#include <cstddef>

namespace egg::Constants::Memory
{
    inline constexpr std::size_t DefaultBytesPerPageElement { 4u };
    static_assert(std::has_single_bit(DefaultBytesPerPageElement), "Size of page element must be a power of two");

    inline constexpr std::size_t PageSizeInBytes { 16384u };
    static_assert(
        std::has_single_bit(PageSizeInBytes) && PageSizeInBytes >= DefaultBytesPerPageElement,
        "Page size must be a power of two and greater than or equal to default number of bytes per page element"
    );

    template <typename Type>
    inline constexpr std::size_t PageSize {
        std::has_single_bit(sizeof(Type))
            ? PageSizeInBytes / sizeof(Type)
            : PageSizeInBytes / DefaultBytesPerPageElement
    };

    inline constexpr std::size_t ByteMultiplier { CHAR_BIT };
    static_assert(std::has_single_bit(ByteMultiplier), "The number of bits in a byte must be a power of two");

    inline constexpr std::size_t ByteShift { std::countr_zero(ByteMultiplier) };

    template <auto Bytes> requires std::unsigned_integral<decltype(Bytes)>
    inline constexpr decltype(Bytes) BitsIn { Bytes << ByteShift };

    template <auto Bits> requires std::unsigned_integral<decltype(Bits)> && Math::MultipleOf<Bits, ByteMultiplier>
    inline constexpr decltype(Bits) BytesIn { Bits >> ByteShift };
}

#endif // ENGINE_SOURCES_CONSTANTS_FILE_MEMORY_H
