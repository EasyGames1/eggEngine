#ifndef ENGINE_SOURCES_ECS_TRAITS_FILE_PAGE_SIZE_TRAITS_H
#define ENGINE_SOURCES_ECS_TRAITS_FILE_PAGE_SIZE_TRAITS_H

#include "BasicEntityTraits.h"
#include "Constants/Memory.h"

#include <type_traits>

namespace egg::ECS
{
    template <typename Type, typename = void>
    struct PageSizeTraits : std::integral_constant<std::size_t, !std::is_empty_v<Type> * Constants::PageSize<Type>>
    {
    };

    template <>
    struct PageSizeTraits<void> : std::integral_constant<std::size_t, 0u>
    {
    };

    template <typename Type>
    struct PageSizeTraits<Type, std::void_t<decltype(Type::PageSize)>>
        : std::integral_constant<std::size_t, Constants::PageSize<typename Type::PageSize>>
    {
    };

    template <ValidEntity Type>
    struct PageSizeTraits<Type> :
        std::integral_constant<typename BasicEntityTraits<Type>::EntityType, Constants::PageSize<Type>>
    {
    };
}


#endif // ENGINE_SOURCES_ECS_TRAITS_FILE_PAGE_SIZE_TRAITS_H
