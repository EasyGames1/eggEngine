#ifndef ENGINE_SOURCES_MATH_FILE_MATH_H
#define ENGINE_SOURCES_MATH_FILE_MATH_H

#include "../Config/Config.h"

#include <bit>
#include <concepts>

namespace egg::Math
{
    template <std::unsigned_integral Type>
    [[nodiscard]] constexpr Type FastModulo(const Type Value, const Type Modulo) noexcept
    {
        EGG_ASSERT(std::has_single_bit(Modulo), "Modulo must be a power of two");
        return Value & Modulo - 1u;
    }
}

#endif // ENGINE_SOURCES_MATH_FILE_MATH_H
