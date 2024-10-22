#ifndef ENGINE_SOURCES_ECS_TRAITS_FILE_BASIC_ENTITY_TRAITS_H
#define ENGINE_SOURCES_ECS_TRAITS_FILE_BASIC_ENTITY_TRAITS_H

#include "../Entity.h"

#include <cstdint>
#include <type_traits>

namespace egg::ECS
{
    template <ValidEntity>
    struct BasicEntityTraits;


    template <ValidEntity Type> requires std::is_class_v<Type> && requires { typename Type::EntityType; }
    struct BasicEntityTraits<Type> : BasicEntityTraits<typename Type::EntityType>
    {
        using ValueType = Type;
    };


    template <ValidEntity Type> requires std::is_enum_v<Type>
    struct BasicEntityTraits<Type> : BasicEntityTraits<std::underlying_type_t<Type>>
    {
        using ValueType = Type;
    };


    template <>
    struct BasicEntityTraits<std::uint32_t>
    {
        using ValueType = std::uint32_t;

        using IntegralType = std::uint32_t;
        using EntityType = std::uint32_t;
        using VersionType = std::uint16_t;

        static constexpr EntityType EntityMask { 0xFFFFFu };
        static constexpr VersionType VersionMask { 0xFFFu };
    };


    template <>
    struct BasicEntityTraits<std::uint64_t>
    {
        using ValueType = std::uint64_t;

        using IntegralType = std::uint64_t;
        using EntityType = std::uint32_t;
        using VersionType = std::uint32_t;

        static constexpr EntityType EntityMask { 0xFFFFFFFFu };
        static constexpr VersionType VersionMask { 0xFFFFFFFFu };
    };
}

#endif // ENGINE_SOURCES_ECS_TRAITS_FILE_BASIC_ENTITY_TRAITS_H
