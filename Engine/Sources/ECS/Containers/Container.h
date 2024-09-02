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

    namespace Traits = egg::Containers::Traits;

    template <typename Type, typename AllocatorType>
    concept ValidAllocator = egg::Containers::ValidAllocator<Type, AllocatorType>;

    template <typename Type>
    concept EBCOEligible = egg::Containers::EBCOEligible<Type>;

    template <typename Type>
    concept IsTransparent = egg::Containers::IsTransparent<Type>;
}


#endif // ENGINE_SOURCES_ECS_CONTAINERS_FILE_CONTAINER_H
