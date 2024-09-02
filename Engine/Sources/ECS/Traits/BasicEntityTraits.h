#ifndef ENGINE_SOURCES_ECS_TRAITS_FILE_BASIC_ENTITY_TRAITS_H
#define ENGINE_SOURCES_ECS_TRAITS_FILE_BASIC_ENTITY_TRAITS_H

#include <cstdint>
#include <type_traits>

namespace egg::ECS
{
    template <typename Type>
    concept ValidEntity = std::is_enum_v<Type> || std::is_unsigned_v<Type> || (std::is_class_v<Type> && requires
    {
        typename Type::EntityType;
    });

    template <ValidEntity>
    struct BasicEntityTraits;

    template <ValidEntity Type> requires std::is_class_v<Type>
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

        static constexpr EntityType EntityMask { 0xFFFFF };
        static constexpr VersionType VersionMask { 0xFFF };
    };

    template <>
    struct BasicEntityTraits<std::uint64_t>
    {
        using ValueType = std::uint64_t;

        using IntegralType = std::uint64_t;
        using EntityType = std::uint32_t;
        using VersionType = std::uint32_t;

        static constexpr EntityType EntityMask { 0xFFFFFFFF };
        static constexpr VersionType VersionMask { 0xFFFFFFFF };
    };
}

#endif // ENGINE_SOURCES_ECS_TRAITS_FILE_BASIC_ENTITY_TRAITS_H
