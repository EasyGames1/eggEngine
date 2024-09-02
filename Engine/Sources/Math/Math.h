#ifndef ENGINE_SOURCES_MATH_FILE_MATH_H
#define ENGINE_SOURCES_MATH_FILE_MATH_H

#include "../Config/Config.h"

#include <bit>

namespace egg::Math
{
    [[nodiscard]] constexpr std::size_t FastModulo(const std::size_t Value, const std::size_t Modulo) noexcept
    {
        EGG_ASSERT(std::has_single_bit(Modulo), "Modulo must be a power of two");
        return Value & Modulo - 1u;
    }
}

#endif // ENGINE_SOURCES_MATH_FILE_MATH_H
