#ifndef ENGINE_SOURCES_MATH_TRAITS_FILE_CAPABILITIES_H
#define ENGINE_SOURCES_MATH_TRAITS_FILE_CAPABILITIES_H

namespace egg::Math
{
    template <auto Number, auto Multiplier>
    concept MultipleOf = !(Number % Multiplier);
}

#endif // ENGINE_SOURCES_MATH_TRAITS_FILE_CAPABILITIES_H
