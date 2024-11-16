#ifndef ENGINE_SOURCES_MEMORY_FILE_DELETER_H
#define ENGINE_SOURCES_MEMORY_FILE_DELETER_H

#include "Containers/CompressedPair/CompressedBase.h"

namespace egg::Memory
{
    template <typename AllocatorType>
    class Deleter : Containers::CompressedBase<AllocatorType>
    {
        using BaseType = Containers::CompressedBase<AllocatorType>;
        using DeleterAllocatorTraits = std::allocator_traits<AllocatorType>;

    public:
        using pointer = typename DeleterAllocatorTraits::pointer;
        using allocator_type = AllocatorType;


        constexpr Deleter() noexcept = default;

        constexpr explicit Deleter(const AllocatorType& Allocator) : BaseType { Allocator }
        {
        }

        constexpr void operator()(pointer Target)
        {
            DeleterAllocatorTraits::destroy(BaseType::Get(), Target);
            DeleterAllocatorTraits::deallocate(BaseType::Get(), Target, 1u);
        }
    };
}

#endif // ENGINE_SOURCES_MEMORY_FILE_DELETER_H
