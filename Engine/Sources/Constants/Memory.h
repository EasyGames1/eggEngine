#ifndef ENGINE_SOURCES_CONSTANTS_FILE_MEMORY_H
#define ENGINE_SOURCES_CONSTANTS_FILE_MEMORY_H

#include <cstddef>

namespace egg::Constants
{
    inline constexpr std::size_t PageSizeInBytes { 16384u };

    template <typename Type> requires (sizeof(Type) != 0u)
    inline constexpr std::size_t PageSize { PageSizeInBytes / sizeof(Type) };
}

#endif // ENGINE_SOURCES_CONSTANTS_FILE_MEMORY_H
