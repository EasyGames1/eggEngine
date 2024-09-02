#ifndef ENGINE_SOURCES_CONSTANTS_WINDOW_H
#define ENGINE_SOURCES_CONSTANTS_WINDOW_H

#include <GLFW/glfw3.h>
#include <string_view>

namespace egg::Constants::Window
{
    inline constexpr std::string_view Name { "EasyGames Engine" };

    using DimensionsType = int;
    using ScalarType = float;
    using CoordinatesType = double;

    inline constexpr DimensionsType InitialWidth { 1600 };
    inline constexpr DimensionsType InitialHeight { 1200 };

    inline constexpr DimensionsType MinWidth { 500 };
    inline constexpr DimensionsType MinHeight { 500 };

    inline constexpr DimensionsType MaxWidth { GLFW_DONT_CARE };
    inline constexpr DimensionsType MaxHeight { GLFW_DONT_CARE };
}

#endif // ENGINE_SOURCES_CONSTANTS_WINDOW_H
