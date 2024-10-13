#ifndef ENGINE_SOURCES_TYPE_TRAITS_FILE_CAPABILITIES_H
#define ENGINE_SOURCES_TYPE_TRAITS_FILE_CAPABILITIES_H

#include <type_traits>

namespace egg::Types
{
    template <typename Type>
    concept EBCOEligible = std::is_empty_v<Type> && !std::is_final_v<Type>;

    template <typename Type>
    concept Transparent = requires
    {
        typename Type::is_transparent;
    };
}

#endif // ENGINE_SOURCES_TYPE_TRAITS_FILE_CAPABILITIES_H
