#ifndef ENGINE_SOURCES_TYPE_TRAITS_FILE_CAPABILITIES_H
#define ENGINE_SOURCES_TYPE_TRAITS_FILE_CAPABILITIES_H

#include "./Internal/IsAnyOf.h"
#include "./Internal/IsInstanceOf.h"

#include <concepts>
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
    concept Decayed = std::same_as<Type, std::decay_t<Type>>;

    template <typename Type, template<typename...> typename Template>
    concept InstanceOf = Internal::IsInstanceOf<Type, Template>::value;

    template <typename Type, typename... Other>
    concept AnyOf = Internal::IsAnyOf<Type, Other...>::value;

    template <typename Type>
    concept Byte = AnyOf<std::remove_cv_t<Type>, std::byte, unsigned char, signed char, char>;
}

#endif // ENGINE_SOURCES_TYPE_TRAITS_FILE_CAPABILITIES_H
