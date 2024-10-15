#ifndef ENGINE_SOURCES_ECS_CONTAINERS_FILE_CONTAINER_H
#define ENGINE_SOURCES_ECS_CONTAINERS_FILE_CONTAINER_H

#include "Containers/Container.h"

namespace egg::ECS::Containers
{
    using egg::Containers::begin;
    using egg::Containers::cbegin;

    using egg::Containers::end;
    using egg::Containers::cend;

    using egg::Containers::rbegin;
    using egg::Containers::crbegin;

    using egg::Containers::rend;
    using egg::Containers::crend;

    template <typename Type>
    using AllocatorTraits = egg::Containers::AllocatorTraits<Type>;

    template <typename Type>
    using IteratorTraits = egg::Containers::IteratorTraits<Type>;
}


#endif // ENGINE_SOURCES_ECS_CONTAINERS_FILE_CONTAINER_H
