#ifndef ENGINE_SOURCES_ECS_TRAITS_FILE_PAGE_SIZE_TRAITS_H
#define ENGINE_SOURCES_ECS_TRAITS_FILE_PAGE_SIZE_TRAITS_H

#include <ECS/Traits/BasicEntityTraits.h>
#include <Memory/Constants.h>

#include <type_traits>

namespace egg::ECS
{
    template <typename Type>
    struct PageSizeTraits : std::integral_constant<std::size_t, !std::is_empty_v<Type> * Memory::PageSize<Type>>
    {
    };

    template <typename Type> requires std::is_void_v<Type>
    struct PageSizeTraits<Type> : std::integral_constant<std::size_t, 0u>
    {
    };

    template <typename Type> requires requires { Type::PageSize; } && std::integral<decltype(Type::PageSize)>
    struct PageSizeTraits<Type> : std::integral_constant<std::size_t, Memory::PageSize<typename Type::PageSize>>
    {
    };

    template <ValidEntity Type>
    struct PageSizeTraits<Type> : std::integral_constant<typename BasicEntityTraits<Type>::EntityType, Memory::PageSize<Type>>
    {
    };
}


#endif // ENGINE_SOURCES_ECS_TRAITS_FILE_PAGE_SIZE_TRAITS_H
