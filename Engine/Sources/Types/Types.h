#ifndef ENGINE_SOURCES_TYPE_FILE_TYPES_H
#define ENGINE_SOURCES_TYPE_FILE_TYPES_H

#include <concepts>
#include <cstdint>

namespace egg::Types
{
    using HashType = std::uint32_t;
    static_assert(std::unsigned_integral<HashType>, "The hash type must be an unsigned integral");

    using TypeID = HashType;
    static_assert(std::unsigned_integral<TypeID>, "The type id must be an unsigned integral");
}

#endif // ENGINE_SOURCES_TYPE_FILE_TYPES_H
