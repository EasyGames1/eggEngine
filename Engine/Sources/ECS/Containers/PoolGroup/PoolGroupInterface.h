#ifndef ENGINE_SOURCES_ECS_CONTAINERS_POOL_GROUP_FILE_POOL_GROUP_INTERFACE_H
#define ENGINE_SOURCES_ECS_CONTAINERS_POOL_GROUP_FILE_POOL_GROUP_INTERFACE_H

#include <cstddef>

namespace egg::ECS::Containers
{
    struct PoolGroupInterface
    {
        constexpr virtual ~PoolGroupInterface() = default;
        [[nodiscard]] constexpr virtual std::size_t GetSize() const noexcept = 0;
        [[nodiscard]] constexpr virtual bool Empty() const noexcept = 0;
    };
}

#endif // ENGINE_SOURCES_ECS_CONTAINERS_POOL_GROUP_FILE_POOL_GROUP_INTERFACE_H
