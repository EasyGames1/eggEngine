#ifndef ENGINE_SOURCES_CONSTANTS_FILE_MEMORY_H
#define ENGINE_SOURCES_CONSTANTS_FILE_MEMORY_H

#include "Math/Math.h"
#include "Math/Traits/Capabilities.h"

#include <cstddef>

namespace egg::Constants::Memory
{
    inline constexpr std::size_t PageSizeInBytes { 16384u };

    template <typename Type> requires (sizeof(Type) != 0u)
    inline constexpr std::size_t PageSize { PageSizeInBytes / sizeof(Type) };

    inline constexpr std::size_t ByteMultiplier { 8u };
    inline constexpr std::size_t ByteShift { 3u };

    template <auto Bytes> requires std::unsigned_integral<decltype(Bytes)>
    inline constexpr decltype(Bytes) BitsIn { Bytes << ByteShift };

    template <auto Bits> requires std::unsigned_integral<decltype(Bits)> && Math::MultipleOf<Bits, ByteMultiplier>
    inline constexpr decltype(Bits) BytesIn { Bits >> ByteShift };
}

#endif // ENGINE_SOURCES_CONSTANTS_FILE_MEMORY_H
