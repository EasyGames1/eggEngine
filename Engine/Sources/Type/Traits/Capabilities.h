#ifndef ENGINE_SOURCES_TYPE_TRAITS_FILE_CAPABILITIES_H
#define ENGINE_SOURCES_TYPE_TRAITS_FILE_CAPABILITIES_H

#include "Deduction.h"

#include <memory>
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

    template <typename AllocatorType, typename Type>
    concept ValidAllocator = std::same_as<Type, typename std::allocator_traits<AllocatorType>::value_type>;

    template <typename Type>
    concept Character = ContainedIn<Type, std::tuple<char, signed char, unsigned char, wchar_t, char8_t, char16_t, char32_t>>;
}

#endif // ENGINE_SOURCES_TYPE_TRAITS_FILE_CAPABILITIES_H
