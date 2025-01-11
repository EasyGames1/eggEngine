#ifndef ENGINE_SOURCES_TYPE_FILE_TYPES_H
#define ENGINE_SOURCES_TYPE_FILE_TYPES_H

#include <concepts>
#include <cstdint>

namespace egg::Types
{
    using IDType = std::uint32_t;
    static_assert(std::unsigned_integral<IDType>, "The type id must be an unsigned integral");
}

#endif // ENGINE_SOURCES_TYPE_FILE_TYPES_H
