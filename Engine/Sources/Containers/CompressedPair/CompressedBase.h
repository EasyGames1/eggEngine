#ifndef ENGINE_SOURCES_CONTAINERS_COMPRESSED_PAIR_FILE_COMPRESSED_BASE_H
#define ENGINE_SOURCES_CONTAINERS_COMPRESSED_PAIR_FILE_COMPRESSED_BASE_H

#include "Internal/CompressedPairElement.h"

namespace egg::Containers
{
    template <typename Type>
    using CompressedBase = Internal::CompressedPairElement<Type, 0u>;
}

#endif // ENGINE_SOURCES_CONTAINERS_COMPRESSED_PAIR_FILE_COMPRESSED_BASE_H
