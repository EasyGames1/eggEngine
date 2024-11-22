#ifndef ENGINE_SOURCES_ECS_FILE_ENTITY_H
#define ENGINE_SOURCES_ECS_FILE_ENTITY_H

#include <concepts>
#include <cstdint>
#include <type_traits>

namespace egg::ECS
{
    enum class Entity : std::uint32_t
    {
    };

    template <typename Type>
    concept ValidEntity = std::is_enum_v<Type> || std::unsigned_integral<Type> || (std::is_class_v<Type> && requires
    {
        typename Type::EntityType;
    });
}

#endif // ENGINE_SOURCES_ECS_FILE_ENTITY_H
